// written by masixiang

#include "librbd/WHOBBS.h"

namespace librbd {

#ifndef MAPPER
#define MAPPER

  class Mapper
  {
  public:
    Mapper(ExtentMap *extent_map_p);

  private:
    ExtentMap *extent_map_p;
  };

#endif
}
