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
    static int get_pool(ExtentMap *extent_map_p, uint64_t off);
    static bool is_in_hdd(ExtentMap *extent_map_p, uint64_t id);
  };

#endif
}
