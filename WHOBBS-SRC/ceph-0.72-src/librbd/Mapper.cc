// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"

#include "librbd/Mapper.h"

namespace librbd {

  Mapper::Mapper(ExtentMap *extent_map_p)
    : extent_map_p(extent_map_p)
  {}

}
