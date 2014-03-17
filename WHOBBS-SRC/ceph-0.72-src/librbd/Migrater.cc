// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include "librbd/ImageCtx.h"
#include "librbd/internal.h"
#include "librbd/WatchCtx.h"

#include <ctime>

#include "librbd/Migrater.h"

namespace librbd {

  Migrater::Migrater(ExtentMap *extent_map_p, ImageCtx *ictx) :
    extent_map_p(extent_map_p), ictx(ictx)
  {}

  void Migrater::do_concurrent_migrate(uint64_t extent_id, int from_pool, int to_pool)
  {
    if(from_pool == HDD_POOL)
      cout << "concurrent migration: extent id = " << extent_id << " "  << "HDD_POOL -> SSD_POOL" << std::endl;
    else
      cout << "concurrent migration: extent id = " << extent_id << " "  << "SSD_POOL -> HDD_POOL" << std::endl;

    time_t begin_time = std::time(NULL);
    int obj_num = EXTENT_SIZE / OBJECT_SIZE;
    uint64_t start_off = (extent_id * EXTENT_SIZE);
    int ret = 0;
   
    /*librados::IoCtx io_ctx_from = ictx->data_ctx[SSD_POOL];
    librados::IoCtx io_ctx_to = ictx->data_ctx[HDD_POOL];
    if(from_pool == HDD_POOL) {
      librados::IoCtx io_ctx_from = ictx->data_ctx[HDD_POOL];
      librados::IoCtx io_ctx_to = ictx->data_ctx[SSD_POOL];
    }*/
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
      //cout << "oid = " << oid << std::endl;
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
    time_t end_time = std::time(NULL);

    //cout << "concurrent migration: extent id = " << extent_id << " "  
     //   	<< "HDD_POOL -> SSD_POOL" << ", time = " << end_time - begin_time << std::endl;
      
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
  
  void Migrater::restore_to_default_pool()
  {
    cout << "restore to the default pool" << std::endl;
    for(std::map<uint64_t, int>::iterator it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
      if(it->second != DEFAULT_POOL) {
        if(DEFAULT_POOL == HDD_POOL)
          do_concurrent_migrate(it->first, SSD_POOL, DEFAULT_POOL);
	else
	  do_concurrent_migrate(it->first, HDD_POOL, DEFAULT_POOL);
      }
    }
  }
}
