// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#include <errno.h>

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include "common/perf_counters.h"

#include "librbd/internal.h"
#include "librbd/WatchCtx.h"

#include "librbd/ImageCtx.h"
#include <pthread.h>

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::ImageCtx: "

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

using ceph::bufferlist;
using librados::snap_t;
using librados::IoCtx;

namespace librbd {
 
  // Extent

  int ImageCtx::init_extentmap()
  {
    cout << "init extentmap" << std::endl;

    extent_map.extent_size = EXTENT_SIZE;
    extent_map.map_size = (size / EXTENT_SIZE);
   
    cout << "extentmap mapsize = " << extent_map.map_size << std::endl;

    std::map<uint64_t, int> *p = &(extent_map.map);
    for(int i = 0; i < extent_map.map_size; i++) {
      (*p).insert( std::pair<uint64_t, int>(i, HDD_POOL));
    }
    return 0;
  }

  int ImageCtx::finilize_extentmap()
  {
    cout << "finilize extentmap" << std::endl;
    restore_to_all_hdd();
    return 0;
  }

  void ImageCtx::start_analyzer()
  {
    std::queue<AnalyzerOp> read_op_queue;
    std::queue<AnalyzerOp> write_op_queue;
    std::list<ImageHitMap> history_list;
    std::list<uint64_t> last_byte_list;
    std::map<uint64_t, int> waiting_list;
    
    // init waiting_list all in hdd pool
    for(int id = 0; id < extent_map.map_size; id++) {
      waiting_list.insert( std::pair<uint64_t, int>((uint64_t)id, -3));
    }
    
    analyzer = new Analyzer(read_op_queue, write_op_queue, history_list, this, last_byte_list, waiting_list);
    pthread_t id;
    int ret = pthread_create(&id, NULL, Analyzer::start, analyzer);
    if(ret != 0) {
      printf("Create pthread error!\n");
      exit(1);
    }
   // pthread_join(id, NULL);
   
  }

  uint64_t ImageCtx::get_extent_id(uint64_t off) 
  {
    uint64_t id = off / extent_map.extent_size;
    return id;
  }

  int ImageCtx::get_pool_decision(uint64_t off, size_t len)
  {
    std::map<uint64_t, int> *p = &(extent_map.map);
    uint64_t extent_id = get_extent_id(off);
    int pool = (*p)[extent_id];
    return pool;
  }
  
  void ImageCtx::dump_perf()
  {
    cout << "dump perfcounters" << std::endl;
    bufferlist bl;
    Formatter *f = new_formatter("json-pretty");
    cct->get_perfcounters_collection()->dump_formatted(f, 0); 
    f->flush(bl);
    delete f;
    bl.append('\0');
    cout << bl.c_str() << std::endl;
    cct->get_perfcounters_collection()->clear();
  }

  // Extent

  // Analyzer
  
  Analyzer::Analyzer(std::queue<AnalyzerOp> read_op_queue, std::queue<AnalyzerOp> write_op_queue,
           std::list<ImageHitMap> history_list, ImageCtx *ictx, std::list<uint64_t> last_byte_list, 
	   std::map<uint64_t, int> waiting_list) 
	   : read_op_queue(read_op_queue),  write_op_queue(write_op_queue), history_list(history_list), ictx(ictx), last_byte_list(last_byte_list), waiting_list(waiting_list)
  {}

  void *Analyzer::start(void *arg)
  {
    //sleep(300);
    Analyzer *analyzer = (Analyzer *)arg;

    int analyze_time = 0;
    while(true) {
      int sleep_time = INTERVAL - analyze_time;
      if(sleep_time > 0) {
        printf("sleep %d seconds\n", sleep_time);
        sleep(sleep_time);
      }
      else
        printf("no sleep time\n");

      time_t start = std::time(NULL);
      analyzer->handle_op_queue();
      time_t finish = std::time(NULL);
      analyze_time = finish - start;
      
    }
    return NULL;
  }
  
  void Analyzer::handle_op_queue() 
  {
    printf("handle_queue\n");
    std::map<uint64_t, uint64_t> read_map;
    std::map<uint64_t, uint64_t> write_map;
    uint64_t reads = 0;
    uint64_t writes = 0;
    uint64_t seq_reads = 0;
    uint64_t seq_writes = 0;
    uint64_t read_bytes = 0;
    uint64_t write_bytes = 0;
    ExtentMap current_extent_map = ictx->extent_map;
    int i;

    // init map
    for(i=0; i<current_extent_map.map_size; i++) {
      read_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
      write_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
    }

    int read_queue_size = read_op_queue.size();
    int write_queue_size = write_op_queue.size();

    // empty ops in queues collected during the lastest interval and construct ImageHitMap
    for(i=0; i<read_queue_size; i++) {
      AnalyzerOp op = read_op_queue.front();
      read_op_queue.pop();
      uint64_t off = op.get_off();
      uint64_t len = op.get_len();

      if(is_seqential(off)) {  // seqential workload
        seq_reads++;
      } else { // random workload
        if(len <= LARGE_READ_SIZE) {
          uint64_t extent_id = off / current_extent_map.extent_size;
          read_map.at(extent_id) ++;
	}
      }
      
      reads++;
      read_bytes += len;
      set_last_byte(off + len);
    }

    for(i=0; i<write_queue_size; i++) {
      AnalyzerOp op = write_op_queue.front();
      write_op_queue.pop();
      uint64_t off = op.get_off();
      uint64_t len = op.get_len();

      if(is_seqential(off)) { // seqential workload
        seq_writes++;
      } else { // random workload
        if(len <= LARGE_WRITE_SIZE) {
          uint64_t extent_id = off / current_extent_map.extent_size;
          write_map.at(extent_id) ++;
	}
      }
      
      writes++;
      write_bytes += len;
      set_last_byte(off + len);
    }

    ImageHitMap *image_hit_map = new ImageHitMap(read_map, write_map, reads, writes, seq_reads, seq_writes, read_bytes, write_bytes, current_extent_map);
   
    // analyze this ImageHitMap
    std::list<uint64_t> result_list = analyze(image_hit_map);

    if(result_list.size() != 0) { // need to migrate
       for(std::list<uint64_t>::iterator it =  result_list.begin(); it != result_list.end(); it++) {
         uint64_t extent_id = *it;
	 int from_pool = current_extent_map.map[extent_id];
	 int to_pool;

	 // set extent_map to migrating mode
	 if(from_pool == HDD_POOL) {
	   to_pool = SSD_POOL;
           ictx->extent_map.map[extent_id] = HDD_TO_SSD;
	 }
	 else {
	   to_pool = HDD_POOL;
           ictx->extent_map.map[extent_id] = SSD_TO_HDD;
	 }

         // do migrate
         ictx->do_concurrent_migrate(extent_id, from_pool, to_pool);
	 
	 // set extent_map to to_pool
	 ictx->extent_map.map[extent_id] = to_pool;
	 image_hit_map->set_extent_pool(extent_id, to_pool);
       }	 
    } else {
      printf("no need migration\n");
    }

    (*image_hit_map).print_map();
    history_list.push_back(*image_hit_map);

  }

  void Analyzer::set_last_byte(uint64_t byte)
  {
    int list_size = last_byte_list.size();
    if(list_size < LAST_BYTE_LIST_SIZE)
      last_byte_list.push_back(byte);
    else {
      last_byte_list.pop_front();
      last_byte_list.push_back(byte);
    }
  }

  bool Analyzer::is_seqential(uint64_t off)
  {
    bool seq = false;
    for(std::list<uint64_t>::iterator it = last_byte_list.begin(); it != last_byte_list.end(); it++) {
      uint64_t distance = 0;
      uint64_t last_byte = *it;

      if (off >= last_byte)
        distance = off - last_byte;
      else 
        distance = last_byte - off;

      //printf("off = %ld, last_byte = %ld, distance = %ld\n", off, last_byte, distance);
      if( distance <= SEQ_DISTANCE ) {
        seq = true;
	break;
      }
    }
    return seq;
  }

  std::list<uint64_t> Analyzer::analyze(ImageHitMap *image_hit_map)
  {
    printf("analyze\n");
    std::list<uint64_t> result_list;
    std::list<uint64_t> candidates_to_ssd = image_hit_map->find_ssd_candidates();
    std::list<uint64_t> candidates_to_hdd = image_hit_map->find_hdd_candidates();
    printf("candidates_to_ssd size = %ld, candidates_to_hdd size = %ld\n", candidates_to_ssd.size(), candidates_to_hdd.size());
    
    // waiting list value range: -3 -2 -1 means in hdd-pool, 1 2 3 means in ssd-pool
    for(std::map<uint64_t, int>::iterator it = waiting_list.begin(); it != waiting_list.end(); it++) {
      int candidate_contribute_to = 0;
      uint64_t id = it->first;
      int value = it->second;
      //printf("waiting_list id = %ld, value = %d\n", id, value);
      
      // check contribute to which side
      for(std::list<uint64_t>::iterator itt = candidates_to_ssd.begin(); itt != candidates_to_ssd.end(); itt++) {
        if(id == *itt) {
	  candidate_contribute_to = 1; // contribute to ssd side
	  break;
	}
      } 
      if(candidate_contribute_to == 0) {
        for(std::list<uint64_t>::iterator ittt = candidates_to_hdd.begin(); ittt != candidates_to_hdd.end(); ittt++) {
          if(id == *ittt) {
  	    candidate_contribute_to = -1; // contribute to hdd side
	   // printf("fucking to contribute to hdd side!\n");
	    break;
  	  }
        }
      }
      
      if(candidate_contribute_to == 1) { // contribute to ssd side
        if(value < 0) { // now in hdd-pool
	  if(value < -1) // -2 or -3
	    waiting_list[id] ++;
	  else { // -1
	    waiting_list[id] = 3;
	    result_list.push_back(id);
	    //printf("id = %ld, migrate to ssd pool\n", id);
	  }  
	} else { // now in ssd-pool
	  if(value < 3) // 1 or 2
	    waiting_list[id] ++;
	}
      } else if(candidate_contribute_to == -1){ // contribute to hdd side
        if(value > 0) { // now in ssd-pool
	  if(value > 1) // 2 or 3
	    waiting_list[id] --;
	  else { // 1
	    waiting_list[id] = -3;
	    result_list.push_back(id);
	    //printf("id = %ld, migrate to hdd pool\n", id);
	  }
	} else { // now in hdd-pool
	  if(value > -3)
	    waiting_list[id] --;
	}
      }
      
      // check value range whether violated
      if(value < -3 || value > 3 || value ==0) {
        printf("value in waiting_list violated!!!\n");
      }
    }
    
    return result_list;
  }

  void Analyzer::add_read_op(AnalyzerOp op)
  {
    read_op_queue.push(op);
  }

  void Analyzer::add_write_op(AnalyzerOp op)
  {
    write_op_queue.push(op);
  }

  AnalyzerOp::AnalyzerOp(time_t time, uint64_t off, uint64_t len) :
    time(time), off(off), len(len)
  {}

  uint64_t AnalyzerOp::get_off()
  {
    return off;
  }

  uint64_t AnalyzerOp::get_len()
  {
    return len;
  }

  // Analyzer

  // ImageHitMap

  ImageHitMap::ImageHitMap(std::map<uint64_t, uint64_t> read_map, std::map<uint64_t, uint64_t> write_map, uint64_t reads, uint64_t writes, uint64_t seq_reads, uint64_t seq_writes, uint64_t read_bytes, uint64_t write_bytes,  ExtentMap extent_map) : read_map(read_map), write_map(write_map), reads(reads), writes(writes), seq_reads(seq_reads), seq_writes(seq_writes),  read_bytes(read_bytes), write_bytes(write_bytes),  extent_map(extent_map)
  {}
  
  void ImageHitMap::print_map()
  {
    printf("\n---------ImageHitMap-----------\n");
    double read_seqness = (double)seq_reads / (double)reads;
    double write_seqness = (double)seq_writes / (double)writes;
    printf("read request iops = %ld, bandwith = %ld, seqness = %lf\n", reads / INTERVAL, read_bytes / INTERVAL, read_seqness);
    printf("write request iops = %ld, bandwith = %ld, seqness = %lf\n", writes / INTERVAL, write_bytes / INTERVAL, write_seqness);
    double ssd_ratio = (double)get_ssd_extent_num() / (double)(get_hdd_extent_num() + get_ssd_extent_num());
    printf("hdd_extents = %ld, ssd_extents = %ld, ssd_ratio = %lf\n", get_hdd_extent_num(), get_ssd_extent_num(), ssd_ratio);
    printf("---------ImageHitMap-----------\n\n");
  }

  void ImageHitMap::print_map_details() 
  {
   for(int id = 0; id < extent_map.map_size; id++) {
     printf("random reads = %ld, writes = %ld\n", read_map[id], write_map[id]);
    } 
  }

  std::list<uint64_t> ImageHitMap::find_ssd_candidates()
  {
    std::list<uint64_t> candidates_to_ssd;
    int map_size = extent_map.map_size;
    for(int id = 0; id < map_size; id++) {
      uint64_t reads = read_map[id];  
      uint64_t writes = write_map[id]; 
      double read_cover = (double)reads / (double)(HDD_READ_THRESHOLD * INTERVAL / map_size);
      double write_cover = (double)writes / (double)(HDD_WRITE_THRESHOLD * INTERVAL / map_size);
      if((read_cover + write_cover) >= SLACK_FACTOR && is_in_hdd(id)) {
        candidates_to_ssd.push_back(id);
      }
    }
    return candidates_to_ssd;
  }

  std::list<uint64_t> ImageHitMap::find_hdd_candidates()
  {
    std::list<uint64_t> candidates_to_hdd;
    int map_size = extent_map.map_size;
    for(int id = 0; id < map_size; id++) {
      uint64_t reads = read_map[id];  
      uint64_t writes = write_map[id]; 
      double read_cover = (double)reads / (double)(HDD_READ_THRESHOLD * INTERVAL / map_size);
      double write_cover = (double)writes / (double)(HDD_WRITE_THRESHOLD * INTERVAL / map_size);
      if((read_cover + write_cover) * SLACK_FACTOR < 1) {
        candidates_to_hdd.push_back(id);
      }
    }
    return candidates_to_hdd;
  }

  bool ImageHitMap::is_in_hdd(uint64_t extent_id)
  {
    int pool = extent_map.map[extent_id];
    if(pool == HDD_POOL)
      return true;
    else
      return false;
  }
  
  uint64_t ImageHitMap::get_hdd_extent_num()
  {
    uint64_t num = 0;
    for(std::map<uint64_t, int>::iterator it=extent_map.map.begin(); it!=extent_map.map.end(); it++) {
      if(it->second == HDD_POOL) {
        num++;
      }
    }
    return num;
  }

  uint64_t ImageHitMap::get_ssd_extent_num()
  {
    uint64_t num = 0;
    for(std::map<uint64_t, int>::iterator it=extent_map.map.begin(); it!=extent_map.map.end(); it++) {
      if(it->second == SSD_POOL) {
        num++;
      }
    }
    return num;
  }

  // find the hdd extent absorbing max read ops 
 /* uint64_t ImageHitMap::get_max_read_extent()
  {
    uint64_t max_num = 0;
    uint64_t extent_id = UINT64_MAX;
    for(std::map<uint64_t, uint64_t>::iterator it=read_map.begin(); it!=read_map.end(); it++) {
      uint64_t id = it->first;
      if(is_in_hdd(id)) {
        uint64_t num = it->second;
        if(num > max_num) {
          max_num = num;
  	  extent_id = id;
        }
      }
    }
    if(extent_id != UINT64_MAX) {
      uint64_t read_ios = get_read_ios();
      if(read_ios != 0){
        double ratio = (double)max_num / (double)read_ios;
        printf("max = %ld, ios = %ld\n", max_num, read_ios);
        printf("max read extent_id = %ld, ratio = %lf\n", extent_id, ratio);
      }
    }
    return extent_id;
  }

  // find the hdd extent absorbing max write ops 
  uint64_t ImageHitMap::get_max_write_extent()
  {
    uint64_t max_num = 0;
    uint64_t extent_id = UINT64_MAX;
    for(std::map<uint64_t, uint64_t>::iterator it=write_map.begin(); it!=write_map.end(); it++) {
      uint64_t id = it->first;
      if(is_in_hdd(id)) {
        uint64_t num = it->second;
//	printf("num = %ld\n", num);
        if(num > max_num) {
          max_num = num;
  	  extent_id = id;
        }
      }
    }
    if(extent_id != UINT64_MAX) {
      uint64_t write_ios = get_write_ios();
      if(write_ios != 0){
        double ratio = (double)max_num / (double)write_ios;
        printf("max = %ld, ios = %ld\n", max_num, write_ios);
        printf("max write extent_id = %ld, ratio = %lf\n", extent_id, ratio);
      }
    }
    return extent_id;
  }

  double ImageHitMap::get_hdd_read_absorbtion()
  {
    uint64_t hdd_read_sum = 0;
    uint64_t read_sum = get_reads();
    for(std::map<uint64_t, uint64_t>::iterator it=read_map.begin(); it!=read_map.end(); it++) {
      uint64_t id = it->first;
      if(is_in_hdd(id)) {
        hdd_read_sum += it->second;
      }
    }
    if(read_sum == 0)
      return (double)read_sum;
    else
      return (double)hdd_read_sum / (double)read_sum;
  }

  double ImageHitMap::get_ssd_read_absorbtion()
  {
    double hdd_read_absorbtion = get_hdd_read_absorbtion();
    if(hdd_read_absorbtion == 0)
      return 0;
    else 
      return (1.0 - hdd_read_absorbtion);
  }

  double ImageHitMap::get_hdd_write_absorbtion()
  {
    uint64_t hdd_write_sum = 0;
    uint64_t write_sum = get_writes();
    for(std::map<uint64_t, uint64_t>::iterator it=write_map.begin(); it!=write_map.end(); it++) {
      uint64_t id = it->first;
      if(is_in_hdd(id)) {
        hdd_write_sum += it->second;
      }
    }
    if(write_sum == 0)
      return (double)write_sum;
    else
      return (double)hdd_write_sum / (double)write_sum;
  }

  double ImageHitMap::get_ssd_write_absorbtion()
  {
    double hdd_write_absorbtion = get_hdd_write_absorbtion();
    if(hdd_write_absorbtion == 0)
      return 0;
    else 
      return (1.0 - hdd_write_absorbtion);
  }
  */

  void ImageHitMap::set_extent_pool(uint64_t extent_id, int pool)
  {
    extent_map.map[extent_id] = pool;
  }

  // ImageHitMap

  // Migrater

  void ImageCtx::do_migrate(uint64_t extent_id, int from_pool, int to_pool)
  {
    printf("doing migrating... extent_id = %ld, from pool %d to pool %d\n", extent_id, from_pool, to_pool);
    time_t begin_time = std::time(NULL);
    int from_pool_num = from_pool - 1;
    int to_pool_num = to_pool - 1;
    int obj_num = EXTENT_SIZE / OBJECT_SIZE; 
    uint64_t start_off = (extent_id * EXTENT_SIZE);
    int ret = 0;
    librados::IoCtx io_ctx_from = data_ctx[from_pool_num];
    librados::IoCtx io_ctx_to = data_ctx[to_pool_num];

    for(int i = 0; i < obj_num; i++) {
      uint64_t off = start_off + OBJECT_SIZE * i;

      // mapping
      object_t oid = map_object(off);
      //cout << "object id = " << oid << std::endl;
      librados::bufferlist buf;

      // reading from from_pool
      librados::AioCompletion *read_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_from.aio_read(oid.name, read_completion, &buf, OBJECT_SIZE, off);
      if(ret < 0) {
        std::cout << "couldn't start read object! error " << ret << std::endl;
      }
      read_completion->wait_for_complete();
      ret = read_completion->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't read object! error " << ret << std::endl;
      }

      // writing to to_pool
      librados::AioCompletion *write_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_to.aio_write_full(oid.name, write_completion, buf);
      if(ret < 0) {
        std::cout << "couldn't start write object! error " << ret << std::endl;
      }
      write_completion->wait_for_complete();
      ret = write_completion->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't write object! error " << ret << std::endl;
      }

      // removing from from_pool
      librados::AioCompletion *remove_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_from.aio_remove(oid.name, remove_completion);
      if(ret < 0) {
        std::cout << "couldn't start remove object! error " << ret << std::endl;
      }
      remove_completion->wait_for_complete();
      ret = remove_completion->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't remove object! error " << ret << std::endl;
      }
    }
    time_t end_time = std::time(NULL);
    //printf("migration time = %ld\n", (end_time - begin_time));
  }

  void ImageCtx::do_concurrent_migrate(uint64_t extent_id, int from_pool, int to_pool)
  {
    printf("doing concurrent migrating... extent_id = %ld, from pool %d to pool %d\n", extent_id, from_pool, to_pool);
    time_t begin_time = std::time(NULL);
    int from_pool_num = from_pool - 1;
    int to_pool_num = to_pool - 1;
    int obj_num = EXTENT_SIZE / OBJECT_SIZE; 
    uint64_t start_off = (extent_id * EXTENT_SIZE);
    int ret = 0;
    librados::IoCtx io_ctx_from = data_ctx[from_pool_num];
    librados::IoCtx io_ctx_to = data_ctx[to_pool_num];

    std::map<object_t, uint64_t> obj_map;
    std::map<object_t, librados::bufferlist> buf_map;
    std::map<object_t, librados::AioCompletion*> read_comp_map;
    std::map<object_t, librados::AioCompletion*> write_comp_map;
    std::map<object_t, librados::AioCompletion*> remove_comp_map;

    // mapping
    for(int i = 0; i < obj_num; i++) {
      uint64_t off = start_off + OBJECT_SIZE * i;
      object_t oid = map_object(off);
      obj_map.insert(std::pair<object_t, uint64_t>(oid, off));
    }

    // reading
    for(std::map<object_t, uint64_t>::iterator it=obj_map.begin(); it!=obj_map.end(); it++) {
      // reading from from_pool
      librados::AioCompletion *read_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_from.aio_read((it->first).name, read_completion, &buf_map[it->first], OBJECT_SIZE, it->second);
      if(ret < 0) {
        std::cout << "couldn't start read object! error " << ret << std::endl;
      }
      read_comp_map.insert(std::pair<object_t, librados::AioCompletion*>(it->first, read_completion));
    }
    // wait reading
    for(std::map<object_t, librados::AioCompletion*>::iterator it=read_comp_map.begin(); it!=read_comp_map.end(); it++) {
      (it->second)->wait_for_complete();
      ret = (it->second)->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't read object! error " << ret << std::endl;
      }
    }

    // writing
    for(std::map<object_t, uint64_t>::iterator it=obj_map.begin(); it!=obj_map.end(); it++) {
      // writing to to_pool
      librados::AioCompletion *write_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_to.aio_write_full((it->first).name, write_completion, buf_map[it->first]);
      if(ret < 0) {
        std::cout << "couldn't start write object! error " << ret << std::endl;
      }
      write_comp_map.insert(std::pair<object_t, librados::AioCompletion*>(it->first, write_completion));
    }
    // wait writing
    for(std::map<object_t, librados::AioCompletion*>::iterator it=write_comp_map.begin(); it!=write_comp_map.end(); it++) {
      (it->second)->wait_for_complete();
      ret = (it->second)->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't write object! error " << ret << std::endl;
      }
    }

    // remove
    for(std::map<object_t, uint64_t>::iterator it=obj_map.begin(); it!=obj_map.end(); it++) {
      // removing from from_pool
      librados::AioCompletion *remove_completion = librados::Rados::aio_create_completion();
      ret = io_ctx_from.aio_remove((it->first).name, remove_completion);
      if(ret < 0) {
        std::cout << "couldn't start remove object! error " << ret << std::endl;
      }
      remove_comp_map.insert(std::pair<object_t, librados::AioCompletion*>(it->first, remove_completion));
    }
    // wait removing
    for(std::map<object_t, librados::AioCompletion*>::iterator it=remove_comp_map.begin(); it!=remove_comp_map.end(); it++) {
      (it->second)->wait_for_complete();
      ret = (it->second)->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't remove object! error " << ret << std::endl;
      }
    }
    time_t end_time = std::time(NULL);
   // printf("concurrent migration time = %ld\n", (end_time - begin_time));
  }

  object_t ImageCtx::map_object(uint64_t off)
  {
    size_t len = OBJECT_SIZE;
    vector<pair<uint64_t,uint64_t> > image_extents(1);
    image_extents[0] = make_pair(off, len);

    map<object_t,vector<ObjectExtent> > object_extents;
    uint64_t buffer_ofs = 0;
    for (vector<pair<uint64_t,uint64_t> >::const_iterator p = image_extents.begin();
         p != image_extents.end(); ++p) {
      uint64_t len = p->second;
      int r = clip_io(this, p->first, &len);
      if(r < 0)
        printf("clip_io error\n");
      Striper::file_to_extents(cct, format_string, &layout,
                 p->first, len, 0, object_extents, buffer_ofs);
      buffer_ofs += len;
    }

    for (map<object_t,vector<ObjectExtent> >::iterator p = object_extents.begin(); p != object_extents.end(); ++p) {
      for (vector<ObjectExtent>::iterator q = p->second.begin(); q != p->second.end(); ++q) {
        return q->oid;
      }
    }

    printf("mapping error\n");
    return NULL;
  }

  void ImageCtx::restore_to_all_hdd()
  {
    printf("restore_to_all_hdd\n");
    for(std::map<uint64_t, int>::iterator it=extent_map.map.begin(); it!=extent_map.map.end(); it++) {
      if(it->second == SSD_POOL) {
        do_concurrent_migrate(it->first, SSD_POOL, HDD_POOL);
      }
    }
  }

  // Migrater

  ImageCtx::ImageCtx(const string &image_name, const string &image_id,
		     const char *snap, IoCtx& p0, IoCtx& p1, bool ro)
    : cct((CephContext*)p0.cct()),
      perfcounter(NULL),
      snap_id(CEPH_NOSNAP),
      snap_exists(true),
      read_only(ro),
      flush_encountered(false),
      exclusive_locked(false),
      name(image_name),
      wctx(NULL),
      refresh_seq(0),
      last_refresh(0),
      md_lock("librbd::ImageCtx::md_lock"),
      cache_lock("librbd::ImageCtx::cache_lock"),
      snap_lock("librbd::ImageCtx::snap_lock"),
      parent_lock("librbd::ImageCtx::parent_lock"),
      refresh_lock("librbd::ImageCtx::refresh_lock"),
      old_format(true),
      order(0), size(0), features(0),
      format_string(NULL),
      id(image_id), parent(NULL),
      stripe_unit(0), stripe_count(0),
      object_cacher(NULL), writeback_handler(NULL), object_set(NULL)
  {
    md_ctx[0].dup(p0);
    data_ctx[0].dup(p0);
    md_ctx[1].dup(p1);
    data_ctx[1].dup(p1);

    memset(&header, 0, sizeof(header));
    memset(&layout, 0, sizeof(layout));
    
    int pool = 0;
    string pname = string("librbd-") + id + string("-") +
      data_ctx[pool].get_pool_name() + string("/") + name;
    if (snap) {
      snap_name = snap;
      pname += "@";
      pname += snap_name;
    }
    perf_start(pname);

    if (cct->_conf->rbd_cache) {
      pthread_t tid = pthread_self();
      cout << "pid: " << tid << " enabling caching..." << std::endl;
      Mutex::Locker l(cache_lock);
      ldout(cct, 20) << "enabling caching..." << dendl;
      writeback_handler = new LibrbdWriteback(this, cache_lock);

      uint64_t init_max_dirty = cct->_conf->rbd_cache_max_dirty;
      if (cct->_conf->rbd_cache_writethrough_until_flush)
	init_max_dirty = 0;
      ldout(cct, 20) << "Initial cache settings:"
		     << " size=" << cct->_conf->rbd_cache_size
		     << " num_objects=" << 10
		     << " max_dirty=" << init_max_dirty
		     << " target_dirty=" << cct->_conf->rbd_cache_target_dirty
		     << " max_dirty_age="
		     << cct->_conf->rbd_cache_max_dirty_age << dendl;

      object_cacher = new ObjectCacher(cct, pname, *writeback_handler, cache_lock,
				       NULL, NULL,
				       cct->_conf->rbd_cache_size,
				       10,  /* reset this in init */
				       init_max_dirty,
				       cct->_conf->rbd_cache_target_dirty,
				       cct->_conf->rbd_cache_max_dirty_age,
				       cct->_conf->rbd_cache_block_writes_upfront);
      object_set = new ObjectCacher::ObjectSet(NULL, data_ctx[0].get_id(), 0);
      object_set->return_enoent = true;
      object_cacher->start();
    }
  }

  
  ImageCtx::~ImageCtx() {
    perf_stop();
    if (object_cacher) {
      delete object_cacher;
      object_cacher = NULL;
    }
    if (writeback_handler) {
      delete writeback_handler;
      writeback_handler = NULL;
    }
    if (object_set) {
      delete object_set;
      object_set = NULL;
    }
    delete[] format_string;
  }

  int ImageCtx::init() {
    int pool = 0;
    int r;
    if (id.length()) {
      old_format = false;
    } else {
      r = detect_format(md_ctx[pool], name, &old_format, NULL);
      if (r < 0) {
	lderr(cct) << "error finding header: " << cpp_strerror(r) << dendl;
	return r;
      }
    }

    if (!old_format) {
      if (!id.length()) {
	r = cls_client::get_id(&md_ctx[pool], id_obj_name(name), &id);
	if (r < 0) {
	  lderr(cct) << "error reading image id: " << cpp_strerror(r)
		     << dendl;
	  return r;
	}
      }

      header_oid = header_name(id);
      r = cls_client::get_immutable_metadata(&md_ctx[pool], header_oid,
					     &object_prefix, &order);
      if (r < 0) {
	lderr(cct) << "error reading immutable metadata: "
		   << cpp_strerror(r) << dendl;
	return r;
      }

      r = cls_client::get_stripe_unit_count(&md_ctx[pool], header_oid,
					    &stripe_unit, &stripe_count);
      if (r < 0 && r != -ENOEXEC && r != -EINVAL) {
	lderr(cct) << "error reading striping metadata: "
		   << cpp_strerror(r) << dendl;
	return r;
      }

      init_layout();
    } else {
      header_oid = old_header_name(name);
    }
    return 0;
  }
  
  void ImageCtx::init_layout()
  {
    int pool = 0;
    if (stripe_unit == 0 || stripe_count == 0) {
      stripe_unit = 1ull << order;
      stripe_count = 1;
    }

    memset(&layout, 0, sizeof(layout));
    layout.fl_stripe_unit = stripe_unit;
    layout.fl_stripe_count = stripe_count;
    layout.fl_object_size = 1ull << order;
    layout.fl_pg_pool = data_ctx[pool].get_id();  // FIXME: pool id overflow?

    delete[] format_string;
    size_t len = object_prefix.length() + 16;
    format_string = new char[len];
    if (old_format) {
      snprintf(format_string, len, "%s.%%012llx", object_prefix.c_str());
    } else {
      snprintf(format_string, len, "%s.%%016llx", object_prefix.c_str());
    }

    // size object cache appropriately
    if (object_cacher) {
      uint64_t obj = cct->_conf->rbd_cache_size / (1ull << order);
      ldout(cct, 10) << " cache bytes " << cct->_conf->rbd_cache_size << " order " << (int)order
		     << " -> about " << obj << " objects" << dendl;
      object_cacher->set_max_objects(obj * 4 + 10);
    }

    ldout(cct, 10) << "init_layout stripe_unit " << stripe_unit
		   << " stripe_count " << stripe_count
		   << " object_size " << layout.fl_object_size
		   << " prefix " << object_prefix
		   << " format " << format_string
		   << dendl;
  }

  void ImageCtx::perf_start(string name) {
    PerfCountersBuilder plb(cct, name, l_librbd_first, l_librbd_last);

    plb.add_u64_counter(l_librbd_rd, "rd");
    plb.add_u64_counter(l_librbd_rd_bytes, "rd_bytes");
    plb.add_time_avg(l_librbd_rd_latency, "rd_latency");
    plb.add_u64_counter(l_librbd_wr, "wr");
    plb.add_u64_counter(l_librbd_wr_bytes, "wr_bytes");
    plb.add_time_avg(l_librbd_wr_latency, "wr_latency");
    plb.add_u64_counter(l_librbd_discard, "discard");
    plb.add_u64_counter(l_librbd_discard_bytes, "discard_bytes");
    plb.add_time_avg(l_librbd_discard_latency, "discard_latency");
    plb.add_u64_counter(l_librbd_flush, "flush");
    plb.add_u64_counter(l_librbd_aio_rd, "aio_rd");
    plb.add_u64_counter(l_librbd_aio_rd_bytes, "aio_rd_bytes");
    plb.add_time_avg(l_librbd_aio_rd_latency, "aio_rd_latency");
    plb.add_u64_counter(l_librbd_aio_wr, "aio_wr");
    plb.add_u64_counter(l_librbd_aio_wr_bytes, "aio_wr_bytes");
    plb.add_time_avg(l_librbd_aio_wr_latency, "aio_wr_latency");
    plb.add_u64_counter(l_librbd_aio_discard, "aio_discard");
    plb.add_u64_counter(l_librbd_aio_discard_bytes, "aio_discard_bytes");
    plb.add_time_avg(l_librbd_aio_discard_latency, "aio_discard_latency");
    plb.add_u64_counter(l_librbd_aio_flush, "aio_flush");
    plb.add_time_avg(l_librbd_aio_flush_latency, "aio_flush_latency");
    plb.add_u64_counter(l_librbd_snap_create, "snap_create");
    plb.add_u64_counter(l_librbd_snap_remove, "snap_remove");
    plb.add_u64_counter(l_librbd_snap_rollback, "snap_rollback");
    plb.add_u64_counter(l_librbd_notify, "notify");
    plb.add_u64_counter(l_librbd_resize, "resize");

    perfcounter = plb.create_perf_counters();
    cct->get_perfcounters_collection()->add(perfcounter);
  }

  void ImageCtx::perf_stop() {
    assert(perfcounter);
    cct->get_perfcounters_collection()->remove(perfcounter);
    delete perfcounter;
  }

  int ImageCtx::get_read_flags(snap_t snap_id) {
    int flags = librados::OPERATION_NOFLAG;
    if (snap_id == LIBRADOS_SNAP_HEAD)
      return flags;

    if (cct->_conf->rbd_balance_snap_reads)
      flags |= librados::OPERATION_BALANCE_READS;
    else if (cct->_conf->rbd_localize_snap_reads)
      flags |= librados::OPERATION_LOCALIZE_READS;
    return flags;
  }

  int ImageCtx::snap_set(string in_snap_name)
  {
    int pool = 0;
    map<string, SnapInfo>::iterator it = snaps_by_name.find(in_snap_name);
    if (it != snaps_by_name.end()) {
      snap_name = in_snap_name;
      snap_id = it->second.id;
      snap_exists = true;
      data_ctx[pool].snap_set_read(snap_id);
      return 0;
    }
    return -ENOENT;
  }

  void ImageCtx::snap_unset()
  {
    int pool = 0;
    snap_id = CEPH_NOSNAP;
    snap_name = "";
    snap_exists = true;
    data_ctx[pool].snap_set_read(snap_id);
  }

  snap_t ImageCtx::get_snap_id(string in_snap_name) const
  {
    map<string, SnapInfo>::const_iterator it = snaps_by_name.find(in_snap_name);
    if (it != snaps_by_name.end())
      return it->second.id;
    return CEPH_NOSNAP;
  }

  int ImageCtx::get_snap_name(snapid_t in_snap_id, string *out_snap_name) const
  {
    map<string, SnapInfo>::const_iterator it;

    for (it = snaps_by_name.begin(); it != snaps_by_name.end(); ++it) {
      if (it->second.id == in_snap_id) {
	*out_snap_name = it->first;
	return 0;
      }
    }
    return -ENOENT;
  }

  int ImageCtx::get_parent_spec(snapid_t in_snap_id, parent_spec *out_pspec)
  {
    map<string, SnapInfo>::iterator it;

    for (it = snaps_by_name.begin(); it != snaps_by_name.end(); ++it) {
      if (it->second.id == in_snap_id) {
	*out_pspec = it->second.parent.spec;
	return 0;
      }
    }
    return -ENOENT;
  }

  uint64_t ImageCtx::get_current_size() const
  {
    return size;
  }

  uint64_t ImageCtx::get_object_size() const
  {
    return 1ull << order;
  }

  string ImageCtx::get_object_name(uint64_t num) const {
    char buf[object_prefix.length() + 32];
    snprintf(buf, sizeof(buf), format_string, num);
    return string(buf);
  }

  uint64_t ImageCtx::get_stripe_unit() const
  {
    return stripe_unit;
  }

  uint64_t ImageCtx::get_stripe_count() const
  {
    return stripe_count;
  }

  uint64_t ImageCtx::get_stripe_period() const
  {
    return stripe_count * (1ull << order);
  }

  uint64_t ImageCtx::get_num_objects() const
  {
    uint64_t period = get_stripe_period();
    uint64_t num_periods = (size + period - 1) / period;
    return num_periods * stripe_count;
  }

  int ImageCtx::is_snap_protected(string in_snap_name, bool *is_protected) const
  {
    map<string, SnapInfo>::const_iterator it = snaps_by_name.find(in_snap_name);
    if (it != snaps_by_name.end()) {
      *is_protected =
	(it->second.protection_status == RBD_PROTECTION_STATUS_PROTECTED);
      return 0;
    }
    return -ENOENT;
  }

  int ImageCtx::is_snap_unprotected(string in_snap_name,
				    bool *is_unprotected) const
  {
    map<string, SnapInfo>::const_iterator it = snaps_by_name.find(in_snap_name);
    if (it != snaps_by_name.end()) {
      *is_unprotected =
	(it->second.protection_status == RBD_PROTECTION_STATUS_UNPROTECTED);
      return 0;
    }
    return -ENOENT;
  }

  void ImageCtx::add_snap(string in_snap_name, snap_t id, uint64_t in_size,
			  uint64_t features,
			  parent_info parent,
			  uint8_t protection_status)
  {
    snaps.push_back(id);
    SnapInfo info(id, in_size, features, parent, protection_status);
    snaps_by_name.insert(pair<string, SnapInfo>(in_snap_name, info));
  }

  uint64_t ImageCtx::get_image_size(snap_t in_snap_id) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      return size;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return 0;
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return 0;
    return p->second.size;
  }

  int ImageCtx::get_features(snap_t in_snap_id, uint64_t *out_features) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      *out_features = features;
      return 0;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return r;
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return -ENOENT;
    *out_features = p->second.features;
    return 0;
  }

  int64_t ImageCtx::get_parent_pool_id(snap_t in_snap_id) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      return parent_md.spec.pool_id;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return -1;
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return -1;
    return p->second.parent.spec.pool_id;
  }

  string ImageCtx::get_parent_image_id(snap_t in_snap_id) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      return parent_md.spec.image_id;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return "";
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return "";
    return p->second.parent.spec.image_id;
  }

  uint64_t ImageCtx::get_parent_snap_id(snap_t in_snap_id) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      return parent_md.spec.snap_id;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return CEPH_NOSNAP;
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return CEPH_NOSNAP;
    return p->second.parent.spec.snap_id;
  }

  int ImageCtx::get_parent_overlap(snap_t in_snap_id, uint64_t *overlap) const
  {
    if (in_snap_id == CEPH_NOSNAP) {
      *overlap = parent_md.overlap;
      return 0;
    }
    string in_snap_name;
    int r = get_snap_name(in_snap_id, &in_snap_name);
    if (r < 0)
      return r;
    map<string, SnapInfo>::const_iterator p = snaps_by_name.find(in_snap_name);
    if (p == snaps_by_name.end())
      return -ENOENT;
    *overlap = p->second.parent.overlap;
    return 0;
  }

  void ImageCtx::aio_read_from_cache(object_t o, bufferlist *bl, size_t len,
				     uint64_t off, Context *onfinish) {
    int pool = 0;
    snap_lock.get_read();
    ObjectCacher::OSDRead *rd = object_cacher->prepare_read(snap_id, bl, 0);
    snap_lock.put_read();
    ObjectExtent extent(o, 0 /* a lie */, off, len, 0);
    extent.oloc.pool = data_ctx[pool].get_id();
    extent.buffer_extents.push_back(make_pair(0, len));
    rd->extents.push_back(extent);
    cache_lock.Lock();
    int r = object_cacher->readx(rd, object_set, onfinish);
    cache_lock.Unlock();
    if (r != 0)
      onfinish->complete(r);
  }

  void ImageCtx::write_to_cache(object_t o, bufferlist& bl, size_t len,
				uint64_t off, Context *onfinish) {
    int pool = 0;
    snap_lock.get_read();
    ObjectCacher::OSDWrite *wr = object_cacher->prepare_write(snapc, bl,
							      utime_t(), 0);
    snap_lock.put_read();
    ObjectExtent extent(o, 0, off, len, 0);
    extent.oloc.pool = data_ctx[pool].get_id();
    // XXX: nspace is always default, io_ctx_impl field private
    //extent.oloc.nspace = data_ctx.io_ctx_impl->oloc.nspace;
    extent.buffer_extents.push_back(make_pair(0, len));
    wr->extents.push_back(extent);
    {
      Mutex::Locker l(cache_lock);
      object_cacher->writex(wr, object_set, cache_lock, onfinish);
    }
  }

  int ImageCtx::read_from_cache(object_t o, bufferlist *bl, size_t len,
				uint64_t off) {
    int r;
    Mutex mylock("librbd::ImageCtx::read_from_cache");
    Cond cond;
    bool done;
    Context *onfinish = new C_SafeCond(&mylock, &cond, &done, &r);
    aio_read_from_cache(o, bl, len, off, onfinish);
    mylock.Lock();
    while (!done)
      cond.Wait(mylock);
    mylock.Unlock();
    return r;
  }

  void ImageCtx::user_flushed() {
    if (object_cacher && cct->_conf->rbd_cache_writethrough_until_flush) {
      md_lock.get_read();
      bool flushed_before = flush_encountered;
      md_lock.put_read();

      uint64_t max_dirty = cct->_conf->rbd_cache_max_dirty;
      if (!flushed_before && max_dirty > 0) {
	md_lock.get_write();
	flush_encountered = true;
	md_lock.put_write();

	ldout(cct, 10) << "saw first user flush, enabling writeback" << dendl;
	Mutex::Locker l(cache_lock);
	object_cacher->set_max_dirty(max_dirty);
      }
    }
  }

  void ImageCtx::flush_cache_aio(Context *onfinish) {
    cache_lock.Lock();
    object_cacher->flush_set(object_set, onfinish);
    cache_lock.Unlock();
  }

  int ImageCtx::flush_cache() {
    int r = 0;
    Mutex mylock("librbd::ImageCtx::flush_cache");
    Cond cond;
    bool done;
    Context *onfinish = new C_SafeCond(&mylock, &cond, &done, &r);
    flush_cache_aio(onfinish);
    mylock.Lock();
    while (!done) {
      ldout(cct, 20) << "waiting for cache to be flushed" << dendl;
      cond.Wait(mylock);
    }
    mylock.Unlock();
    ldout(cct, 20) << "finished flushing cache" << dendl;
    return r;
  }

  void ImageCtx::shutdown_cache() {
    md_lock.get_write();
    invalidate_cache();
    md_lock.put_write();
    object_cacher->stop();
  }

  void ImageCtx::invalidate_cache() {
    if (!object_cacher)
      return;
    cache_lock.Lock();
    object_cacher->release_set(object_set);
    cache_lock.Unlock();
    int r = flush_cache();
    if (r)
      lderr(cct) << "flush_cache returned " << r << dendl;
    cache_lock.Lock();
    bool unclean = object_cacher->release_set(object_set);
    cache_lock.Unlock();
    if (unclean)
      lderr(cct) << "could not release all objects from cache" << dendl;
  }

  void ImageCtx::clear_nonexistence_cache() {
    if (!object_cacher)
      return;
    cache_lock.Lock();
    object_cacher->clear_nonexistence(object_set);
    cache_lock.Unlock();
  }

  int ImageCtx::register_watch() {
    int pool = 0;
    assert(!wctx);
    wctx = new WatchCtx(this);
    return md_ctx[pool].watch(header_oid, 0, &(wctx->cookie), wctx);
  }

  void ImageCtx::unregister_watch() {
    int pool = 0;
    assert(wctx);
    wctx->invalidate();
    md_ctx[pool].unwatch(header_oid, wctx->cookie);
    delete wctx;
    wctx = NULL;
  }

  size_t ImageCtx::parent_io_len(uint64_t offset, size_t length,
				 snap_t in_snap_id)
  {
    uint64_t overlap = 0;
    get_parent_overlap(in_snap_id, &overlap);

    size_t parent_len = 0;
    if (get_parent_pool_id(in_snap_id) != -1 && offset <= overlap)
      parent_len = min(overlap, offset + length) - offset;

    ldout(cct, 20) << __func__ << " off = " << offset << " len = " << length
		   << " overlap = " << overlap << " parent_io_len = "
		   << parent_len << dendl;
    return parent_len;
  }

  uint64_t ImageCtx::prune_parent_extents(vector<pair<uint64_t,uint64_t> >& objectx,
					  uint64_t overlap)
  {
    // drop extents completely beyond the overlap
    while (!objectx.empty() && objectx.back().first >= overlap)
      objectx.pop_back();

    // trim final overlapping extent
    if (!objectx.empty() && objectx.back().first + objectx.back().second > overlap)
      objectx.back().second = overlap - objectx.back().first;

    uint64_t len = 0;
    for (vector<pair<uint64_t,uint64_t> >::iterator p = objectx.begin();
	 p != objectx.end();
	 ++p)
      len += p->second;
    ldout(cct, 10) << "prune_parent_extents image overlap " << overlap
		   << ", object overlap " << len
		   << " from image extents " << objectx << dendl;
    return len;
 }
}
