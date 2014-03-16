// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include <ctime>

#include "librbd/Analyzer.h"
#include "librbd/Mapper.h"

namespace librbd {

  Analyzer::Analyzer(ExtentMap *extent_map_p) 
    : extent_map_p(extent_map_p)
  {}

  void *Analyzer::startAnalyzer(void *arg)
  {
    Analyzer *analyzer = (Analyzer *)arg
    ;
    cout << "startAnalyzer" << std::endl;
    int analyze_time = 0;
    while(true) {
      int sleep_time = INTERVAL - analyze_time;
      if(sleep_time > 0) {
        cout << "sleep " << sleep_time << " seconds" << std::endl;
	sleep(sleep_time);
    }
    else
      cout << "no sleep time" << std::endl;

      time_t start = std::time(NULL);
      analyzer->handle();
      time_t finish = std::time(NULL);
      analyze_time = finish - start;
    }
    return NULL;
  }

  void Analyzer::add_op(AnalyzerOp op) 
  {
    op_queue.push(op);
  }

  void Analyzer::handle()
  {
    cout << "handle" << std::endl;

    // create a report and print it
    AnalyzerReport *report = create_report(extent_map_p, &op_queue);
    report->print_report();
    
    // analyze the report by WHOBBS Placement Model
    std::map<int, std::list<uint64_t> > placement_map = analyze_placement(report);

    cout << "placement: " << placement_map[HDD_POOL].size() << " extent -> hdd pool" << ", " 
    	<< placement_map[SSD_POOL].size() << " extent -> ssd pool" << std::endl;

    for(std::list<uint64_t>::iterator it = placement_map[HDD_POOL].begin(); it != placement_map[HDD_POOL].end(); it++) {
      uint64_t extent_id = *it;
      //do_concurrent_migration(extent_id, SSD_POOL, HDD_POOL);
    }
    
    for(std::list<uint64_t>::iterator it = placement_map[SSD_POOL].begin(); it != placement_map[SSD_POOL].end(); it++) {
      uint64_t extent_id = *it;
      //do_concurrent_migration(extent_id, HDD_POOL, SSD_POOL);
    }
  }

  AnalyzerReport* Analyzer::create_report(ExtentMap *extent_map_p, std::queue<AnalyzerOp> *op_queue)
  {
    std::map<uint64_t, uint64_t> read_score_map;
    std::map<uint64_t, uint64_t> write_score_map;
    uint64_t ran_reads = 0;
    uint64_t ran_writes = 0;
    uint64_t seq_reads = 0;
    uint64_t seq_writes = 0;
    uint64_t read_bytes = 0;
    uint64_t write_bytes = 0;

    uint64_t i;
    for(i=0; i < extent_map_p->map_size; i++) {
      read_score_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
      write_score_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
    }

    uint64_t queue_size = op_queue->size();
    // creating the report
    for(i=0; i < queue_size; i++) {
      AnalyzerOp op = op_queue->front();
      op_queue->pop();

      bool type = op.get_type();
      uint64_t off = op.get_off();
      uint64_t len = op.get_len();

      // calculate the score of the extent by a certain algorithm
      double score = calculate_score(&op);
      uint64_t extent_id = off / extent_map_p->extent_size;
      if(type == READ_OP)
        read_score_map.at(extent_id) += score;
      else
        write_score_map.at(extent_id) += score;

      // update statistics
      if(type == READ_OP) {
        if(is_sequential(off)) {
	  seq_reads ++;
	} else {
	  ran_reads ++;
	}
	read_bytes += len;
      } else {
        if(is_sequential(off)) {
	  seq_writes ++;
	} else {
	  ran_writes ++;
	}
	write_bytes += len;
      }
      set_last_byte(off + len);
    }

    AnalyzerReport *report = new AnalyzerReport(extent_map_p, read_score_map, write_score_map, ran_reads, ran_writes, seq_reads, seq_writes, read_bytes, write_bytes);   
    return report;
  }

  double Analyzer::calculate_score(AnalyzerOp *op)
  {
    double score = 0;
    bool type = op->get_type();
    uint64_t off = op->get_off();
    uint64_t len = op->get_len();

    if(is_sequential(off)) {
      score = 0;
    } else {
      if(type == READ_OP) {
        if(len <= SMALL_READ_SIZE) {
          score = 1;
        }
      } else {
        if(len <= SMALL_WRITE_SIZE) {
          score = 1;
        }
      }
    }
    return score;
  }

  bool Analyzer::is_sequential(uint64_t off)
  {
    bool seq = false;
    for(std::list<uint64_t>::iterator it = last_byte_list.begin(); it != last_byte_list.end(); it++) {
      uint64_t distance = 0;
      uint64_t last_byte = *it;
      
      if (off >= last_byte)
        distance = off - last_byte;
      else
        distance = last_byte - off;
      
      if( distance <= SEQ_DISTANCE ) {
        seq = true;
	break;
      }
    }
    return seq;
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

  std::map<int, std::list<uint64_t> > Analyzer::analyze_placement(AnalyzerReport *report)
  {
    std::map<int, std::list<uint64_t> > placement_map;
    std::list<uint64_t> hdd_placement;
    std::list<uint64_t> ssd_placement;
    std::list<uint64_t> tend_to_hdd = report->tend_to(HDD_POOL);
    std::list<uint64_t> tend_to_ssd = report->tend_to(SSD_POOL);
    cout << "tend to hdd: " << tend_to_hdd.size()  << ", tend to ssd: " << tend_to_ssd.size()  << std::endl;

    for(std::map<uint64_t, int>::iterator it = extent_map_p->map.begin(); it != extent_map_p->map.end(); it++) {
      int tend = HDD_POOL;
      uint64_t id = it->first;
      int value = it->second;

      for(std::list<uint64_t>::iterator it = tend_to_ssd.begin(); it != tend_to_ssd.end(); it++) {
        if(id == *it) {
	  tend = SSD_POOL;
	  break;
	}
      }

      if(tend == SSD_POOL) {
        if(value < 0) {
	  if(value < -1)
	    extent_map_p->map[id] ++;
	  else
	    ssd_placement.push_back(id);
	} else {
	  if(value < SSD_STRIDE)
	    extent_map_p->map[id] ++;
	}
      } else {
        if(value > 0) {
	  if(value > 1)
	    extent_map_p->map[id] --;
	  else 
	    hdd_placement.push_back(id);
	} else {
	  if(value > HDD_STRIDE)
	    extent_map_p->map[id] --;
	}
      }
      
      if(value < HDD_STRIDE || value > SSD_STRIDE || value == 0)
        cout << "value is in violation!" << std::endl;
    } 
    placement_map.insert(std::pair<int, std::list<uint64_t> >(HDD_POOL, hdd_placement));
    placement_map.insert(std::pair<int, std::list<uint64_t> >(SSD_POOL, ssd_placement));
    return placement_map;
  }

  //--- AnalyzerReport ---//

  AnalyzerReport::AnalyzerReport()
  {}

  AnalyzerReport::AnalyzerReport(ExtentMap *extent_map_p, std::map<uint64_t, uint64_t> read_score_map, std::map<uint64_t, uint64_t> write_score_map, uint64_t ran_reads, uint64_t ran_writes, uint64_t seq_reads, uint64_t seq_writes, uint64_t read_bytes, uint64_t write_bytes) :
  	extent_map_p(extent_map_p), read_score_map(read_score_map), write_score_map(write_score_map), ran_reads(ran_reads), ran_writes(ran_writes), seq_reads(seq_reads), seq_writes(seq_writes), read_bytes(read_bytes), write_bytes(write_bytes)
  {}

  void AnalyzerReport::print_report()
  {
    cout << "---------------Report---------------" << std::endl;
    double read_seqness = (double)seq_reads / ((double)seq_reads + (double)ran_reads);
    double write_seqness = (double)seq_writes / ((double)seq_writes + (double)ran_writes);
    uint64_t ssd_extent_num = Mapper::ssd_extent_num(extent_map_p);
    uint64_t hdd_extent_num = Mapper::hdd_extent_num(extent_map_p);
    double ssd_ratio = (double)ssd_extent_num / ((double)ssd_extent_num + (double)hdd_extent_num);
    cout << "read iops = " << (ran_reads + seq_reads) / INTERVAL << ", bandwidth = "
    	<< read_bytes / INTERVAL << ", seqness = " << read_seqness << std::endl;
    cout << "write iops = " << (ran_writes + seq_writes) / INTERVAL << ", bandwidth = "
    	<< write_bytes / INTERVAL << ", seqness = " << write_seqness << std::endl;
    cout << "hdd extent num = " << hdd_extent_num << ", ssd extent num = " << ssd_extent_num
    	<< ", ssd ratio = " << ssd_ratio << std::endl;
    cout << "---------------Report---------------" << std::endl;
    cout << "" << std::endl;
  }

  std::list<uint64_t> AnalyzerReport::tend_to(int pool)
  {
    std::list<uint64_t> tend_list; 
    int map_size = extent_map_p->map_size;
    std::map<uint64_t, int> map = extent_map_p->map;

    for(int id = 0; id < map_size; id++) {
      uint64_t read_score = read_score_map[id];
      uint64_t write_score = write_score_map[id];
      uint64_t extent_size = extent_map_p->extent_size / (1024 * 1024); // unit of MB
      double read_cover = (read_score * HDD_CAPACITY) / (HDD_READ_IOPS * extent_size * INTERVAL * REPLICA);
      double write_cover = (write_score * HDD_CAPACITY) / (HDD_WRITE_IOPS * extent_size * INTERVAL * REPLICA);
      double total_cover = read_cover + write_cover;

      if(pool == SSD_POOL) {
        if(total_cover >= 1)
	  tend_list.push_back(id);
      } else {
        if(total_cover < 1) {
	  tend_list.push_back(id);
	}
      }
    }
    return tend_list;
  }
  
  //--- AnalyzerOp ---//
  AnalyzerOp::AnalyzerOp(bool type, time_t time, uint64_t off, uint64_t len) :
    time(time), off(off), len(len)
  {}

  bool AnalyzerOp::get_type()
  {
    return type;
  }

  uint64_t AnalyzerOp::get_off()
  {
    return off;
  }
  
  uint64_t AnalyzerOp::get_len()
  {
    return len;
  }

}
