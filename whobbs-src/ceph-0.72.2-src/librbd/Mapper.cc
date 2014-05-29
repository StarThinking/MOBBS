// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include "librbd/ImageCtx.h"

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

  librados::bufferlist Mapper::fetch_extent_table(ImageCtx *ictx, std::string object_name)
  {
    cout << "fetch_extent_table"  << std::endl;
    int ret;
    librados::bufferlist read_buf;
    librados::IoCtx io_ctx = ictx->data_ctx[HDD_POOL];

    ret = io_ctx.read(object_name, read_buf, OBJECT_SIZE, 0);
    if(ret < 0) {
      std::cout << "couldn't read object! error " << ret << std::endl;
    }
    //cout << "read_buf = " << read_buf.c_str() <<std::endl;
    std::string buf_str = read_buf.c_str();
    std::stringstream str(buf_str);
    std::string tok;
    std::string id;
    std::string value;
    int counter = 0;
    std::map<uint64_t, int> *p = &(ictx->extent_map.map);

    while(getline(str, tok, '#')) {
      if(counter % 2 == 0) {
        id = tok;
      }
      else {
        value = tok;
	uint64_t int_id;
	int int_value;
	std::stringstream id_stream(id);
	std::stringstream value_stream(value);
	id_stream >> int_id;
	value_stream >> int_value;
	//cout << "id = " << id << ", value = " << value << std::endl;
        (*p).insert(std::pair<uint64_t, int>(int_id, int_value));
      }
      counter ++;
    }
    //cout << "counter = " << counter << std::endl;

    /*for(std::map<uint64_t, int>::iterator it = ictx->extent_map.map.begin(); it != ictx->extent_map.map.end(); it++) {
      uint64_t id = it->first;
      int value = it->second;
      //cout << "id = " << id << ", value = " << value << std::endl;
    }*/
    return read_buf;
  }

  void Mapper::save_extent_table(ImageCtx *ictx, std::string object_name)
  {
    cout << "save_extent_table" << std::endl;
    librados::bufferlist bl;
    librados::bufferlist read_buf;
    int ret;
    librados::IoCtx io_ctx = ictx->data_ctx[HDD_POOL];

    for(std::map<uint64_t, int>::iterator it = ictx->extent_map.map.begin(); it != ictx->extent_map.map.end(); it++) {
      uint64_t id = it->first;
      int value = it->second;
      std::stringstream id_ss, value_ss;
      std::string id_str, value_str;
      id_ss << id;
      id_ss >> id_str;
      value_ss << value;value_ss >> value_str;
      bl.append(id_str);
      bl.append("#");
      bl.append(value_str);
      bl.append("#");
    }

    //cout << "extent-table: \n" << bl.c_str() << std::endl;
    ret = io_ctx.write_full(object_name, bl);
    if(ret < 0) {
      std::cout << "couldn't write object! error " << ret << std::endl;
    } else {
    }  
  }

}
