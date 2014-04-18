// written by masixiang

#include "librbd/WHOBBS.h"

namespace librbd {

#ifndef MAPPER
#define MAPPER

  class Mapper
  {
  public:
    static uint64_t ssd_extent_num(ExtentMap *extent_map_p);
    static uint64_t hdd_extent_num(ExtentMap *extent_map_p);
    static int get_pool(string type, ExtentMap *extent_map_p, uint64_t off, uint64_t len);
    static bool is_in_hdd(ExtentMap *extent_map_p, uint64_t id);
    static librados::bufferlist fetch_extent_table(ImageCtx *ictx, std::string object_name);
    static void save_extent_table(ImageCtx *ictx, std::string object_name);
  };

#endif
}
