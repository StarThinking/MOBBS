#ifndef CEPH_RADOS_TYPES_HPP
#define CEPH_RADOS_TYPES_HPP

#include <utility>
#include <vector>
#include <stdint.h>

namespace librados {

typedef uint64_t snap_t;

enum {
  SNAP_HEAD = (uint64_t)(-2),
  SNAP_DIR = (uint64_t)(-1)
};

struct clone_info_t {
  snap_t cloneid;
  std::vector<snap_t> snaps;          // ascending
  std::vector< std::pair<uint64_t,uint64_t> > overlap;  // with next newest
  uint64_t size;
};

struct snap_set_t {
  std::vector<clone_info_t> clones;   // ascending
  snap_t seq;   // newest snapid seen by the object
};

}
#endif
