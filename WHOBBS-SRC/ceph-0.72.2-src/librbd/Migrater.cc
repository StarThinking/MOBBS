// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include "librbd/ImageCtx.h"
#include "librbd/internal.h"
#include "librbd/WatchCtx.h"

#include <ctime>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>

#include "librbd/Migrater.h"

namespace librbd {

  Migrater::Migrater(ExtentMap *extent_map_p, ImageCtx *ictx) :
    extent_map_p(extent_map_p), ictx(ictx), migrating(false), to_finilize(false)
  {}

  /*extern "C" int osd_migrate()
  {
    char *argv[] = {"hello"};
    if(execl("./migrater/TutorialClient", argv[0], NULL) < 0) {
      printf("execl error\n");
    }
    return 0;
  }

  int Migrater::do_osd_migrate(uint64_t extent_id, int from_pool, int to_pool)
  {
    printf("do_osd_migrate\n");
    pid_t pid;
    pid = fork();
    if(pid < 0) {
      printf("fork error!\n");
      return -1;
    } else if(pid == 0) {
        printf("in son pthread\n");
	osd_migrate();
    } else {
      wait(NULL);
    }
    return 0;
  }*/

  void Migrater::do_concurrent_migrate(uint64_t extent_id, int from_pool, int to_pool)
  {
    pthread_t tid = pthread_self();
    if(from_pool == HDD_POOL)
      cout << "tid = " << tid << ", concurrent migration: extent id = " << extent_id << " "  
       	<< "HDD_POOL -> SSD_POOL" << std::endl;
    else
      cout << "tid = " << tid << ", concurrent migration: extent id = " << extent_id << " "  
       	<< "SSD_POOL -> HDD_POOL" << std::endl;

    //time_t begin_time = std::time(NULL);
    struct timeval tv_begin, tv_end;
    gettimeofday(&tv_begin, NULL);
    int obj_num = EXTENT_SIZE / OBJECT_SIZE;
    uint64_t start_off = (extent_id * EXTENT_SIZE);
    int ret = 0;
   
    librados::IoCtx io_ctx_from = ictx->data_ctx[from_pool];
    librados::IoCtx io_ctx_to = ictx->data_ctx[to_pool];

    std::map<object_t, uint64_t> obj_map;
    std::map<object_t, librados::bufferlist> buf_map;
    std::map<object_t, librados::AioCompletion*> read_comp_map;
    std::map<object_t, librados::AioCompletion*> write_comp_map;
    std::map<object_t, librados::AioCompletion*> remove_comp_map;

    // mapping
    for(int i = 0; i < obj_num; i++) {
      uint64_t off = start_off + OBJECT_SIZE * i;
      object_t oid = map_object(off);
      cout << "migration oid = " << oid << std::endl;;
      obj_map.insert(std::pair<object_t, uint64_t>(oid, off));
    }

    // lock extent map
    if(to_pool == SSD_POOL)
      extent_map_p->map[extent_id] = MIGRATING_TO_SSD;
    else
      extent_map_p->map[extent_id] = MIGRATING_TO_HDD;
    
    // flush the aios towards the original pool
    io_ctx_from.aio_flush();


    // reading
    for(std::map<object_t, uint64_t>::iterator it=obj_map.begin(); it!=obj_map.end(); it++) {
      // reading from from_pool
      librados::AioCompletion *read_completion = librados::Rados::aio_create_completion();
      //ret = io_ctx_from.aio_read((it->first).name, read_completion, &buf_map[it->first], OBJECT_SIZE, it->second);
      ret = io_ctx_from.aio_read((it->first).name, read_completion, &buf_map[it->first], OBJECT_SIZE, 0);
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
    for(std::map<object_t, librados::AioCompletion*>::iterator it=write_comp_map.begin(); it!=write_comp_map.end(); it++){
      (it->second)->wait_for_complete();
      ret = (it->second)->get_return_value();
      if(ret < 0) {
        std::cout << "couldn't write object! error " << ret << std::endl;
      }
    }

    // update extent map
    if(to_pool == HDD_POOL)
      extent_map_p->map[extent_id] = HDD_STRIDE;
    else
      extent_map_p->map[extent_id] = SSD_STRIDE;

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
    
    //time_t end_time = std::time(NULL);
    gettimeofday(&tv_end, NULL);
    long time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec); //us
    time_used /= 1000; // ms
   
    if(from_pool == HDD_POOL)
      cout << "tid = " << tid << ", concurrent migration: extent id = " << extent_id << " "  
       	<< "HDD_POOL -> SSD_POOL" << ", time = " << time_used << " ms" << std::endl;
    else
      cout << "tid = " << tid << ", concurrent migration: extent id = " << extent_id << " "  
       	<< "SSD_POOL -> HDD_POOL" << ", time = " << time_used << " ms" << std::endl;
    
  }

  
  object_t Migrater::map_object(uint64_t off)
  {
    size_t len = OBJECT_SIZE;
    vector<pair<uint64_t,uint64_t> > image_extents(1);
    image_extents[0] = make_pair(off, len);
    map<object_t,vector<ObjectExtent> > object_extents;
    uint64_t buffer_ofs = 0;
    for (vector<pair<uint64_t,uint64_t> >::const_iterator p = image_extents.begin(); p != image_extents.end(); ++p) {
      uint64_t len = p->second;
      int r = clip_io(ictx, p->first, &len);
      if(r < 0)
        printf("clip_io error\n");
      
      Striper::file_to_extents(ictx->cct, ictx->format_string, &(ictx->layout), p->first, len, 0, object_extents, buffer_ofs);
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

  bool Migrater::get_migrating()
  {
    return migrating;
  }

  void Migrater::set_migrating(bool _migrating)
  {
    migrating = _migrating;
  }

  void Migrater::restore_to_default_pool()
  {
    // wrong logical!!!!!!
    cout << "restore to the default pool" << std::endl;
    
    // waiting for migration finishing
    bool migrating = true;
    do{
      std::map<uint64_t, int>::iterator it;
      for(it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
        if(it->second == MIGRATING_TO_SSD || it->second == MIGRATING_TO_HDD) {
	  cout << "waiting for migration finishing before restoring, sleep 10 seconds" << std::endl;
	  sleep(10);
	  break;
	}
      }
      if(it == extent_map_p->map.end()) 
        migrating = false;
    } while(migrating);

    if(DEFAULT_POOL == HDD_POOL) {
      for(std::map<uint64_t, int>::iterator it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
        if(it->second > 0) 
	  do_concurrent_migrate(it->first, SSD_POOL, HDD_POOL);
      }
    } else {
      for(std::map<uint64_t, int>::iterator it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
        if(it->second < 0) 
	  do_concurrent_migrate(it->first, HDD_POOL, SSD_POOL);
      }
    }
  }
}
