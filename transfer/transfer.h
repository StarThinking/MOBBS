#include <rbd/librbd.h>
#include <stdlib.h>

typedef struct RBDState {
    rados_t cluster;
    rados_ioctx_t io_ctx;
    rbd_image_t image;
    const char *pool_name;
    const char *rbd_name;
    const char *snap;
} RBDState;
  
