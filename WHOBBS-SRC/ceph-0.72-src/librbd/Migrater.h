// written by masixiang

#include "librbd/WHOBBS.h"
#include "librbd/ImageCtx.h"

namespace librbd {

#ifndef MIGRATER
#define MIGRATER

  class Migrater
  {
  public:
    Migrater(ExtentMap *extent_map_p, ImageCtx *ictx);
    void do_concurrent_migrate(uint64_t extent_id, int from_pool, int to_pool);
    void restore_to_default_pool();

  protected:
    object_t map_object(uint64_t off);

  private:
   ExtentMap *extent_map_p;
   ImageCtx *ictx;
  };

#endif
}
