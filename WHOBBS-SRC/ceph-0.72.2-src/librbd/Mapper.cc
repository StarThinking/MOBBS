// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"

#include "librbd/Mapper.h"

namespace librbd {

  uint64_t Mapper::hdd_extent_num(ExtentMap *extent_map_p)
  {
    uint64_t num = 0;
    for(std::map<uint64_t, int>::iterator it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
      if(it->second < 0) {
        num++;
      }
    }
    return num;
  }
  
  uint64_t Mapper::ssd_extent_num(ExtentMap *extent_map_p)
  {
    uint64_t num = 0;
    for(std::map<uint64_t, int>::iterator it=extent_map_p->map.begin(); it!=extent_map_p->map.end(); it++) {
      if(it->second > 0) {
        num++;
      }
    }
    return num;
  }

  int Mapper::get_pool(string type, ExtentMap *extent_map_p, uint64_t off, uint64_t len)
  {
    uint64_t extent_id = off / extent_map_p->extent_size;
    int value = (extent_map_p->map)[extent_id];
    int pool = 0;

    // check if during migrating
    if(value == MIGRATING_TO_SSD || value == MIGRATING_TO_HDD) {
     /* if(type.compare("read")) { // read is allowed during migration
        if(value == MIGRATING_TO_SSD)
	  pool = HDD_POOL;
	else
	  pool = SSD_POOL;
      } else { // write will be blocked during migration*/
        pool = value;
      //}
    } else {
      if(value < 0)
        pool = HDD_POOL;
      else if(value > 0)
        pool = SSD_POOL;
      else {
        cout << "wrong value in extent map!" << std::endl;
      }
    }

    // check if cross two extents
    uint64_t bound = (extent_id + 1) * extent_map_p->extent_size;
    if((off + len) > bound)
      cout << "cross the bound!!!" << std::endl;

    return pool;
  }

  bool is_in_hdd(ExtentMap *extent_map_p, uint64_t id)
  {
    int value = (extent_map_p->map)[id];
    if(value < 0)
      return true;
    else
      return false;
  }

}
