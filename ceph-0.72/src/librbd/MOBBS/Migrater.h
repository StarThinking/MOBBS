#include <map>
#include <string>
#include "librbd/MOBBS/MOBBS.h"
#include "librbd/ImageCtx.h"

#ifndef MIGRATER
#define MIGRATER

namespace librbd {

  class Migrater
  {
  public:
    Migrater(ImageCtx *ictx);
    void do_concurrent_migrate(std::string extent_id, int from_pool, int to_pool);
    bool get_migrating();
    void set_migrating(bool migrating);
    bool to_finilize;
  protected:
    object_t map_object(uint64_t off);

  private:
   ImageCtx *ictx;
   bool migrating;
  };

}
#endif
