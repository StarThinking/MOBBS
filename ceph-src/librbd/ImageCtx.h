// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#ifndef CEPH_LIBRBD_IMAGECTX_H
#define CEPH_LIBRBD_IMAGECTX_H

#include <inttypes.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "common/Mutex.h"
#include "common/RWLock.h"
#include "common/snap_types.h"
#include "include/buffer.h"
#include "include/rbd/librbd.hpp"
#include "include/rbd_types.h"
#include "include/types.h"
#include "osdc/ObjectCacher.h"

#include "cls/rbd/cls_rbd_client.h"
#include "librbd/LibrbdWriteback.h"
#include "librbd/SnapInfo.h"
#include "librbd/parent_types.h"
#include "librbd/Extent.h"

class CephContext;
class PerfCounters;

namespace librbd {

  class WatchCtx;

  // Extent

  struct ExtentMap {
    uint64_t extent_size;
    int map_size;
    std::map<uint64_t, int> map;
  };

  // Extent

  // Analyzer

  class AnalyzerOp;
  class ImageHitMap;

  class Analyzer
  {
  public:
    Analyzer(std::queue<AnalyzerOp> read_op_queue, std::queue<AnalyzerOp> write_op_queue, 
       std::list<ImageHitMap> history_list, int INTERVAL, ImageCtx *ictx);
    static void *start(void *arg);
    void add_read_op(AnalyzerOp op);
    void add_write_op(AnalyzerOp op);
    void handle_op_queue();
    std::list<uint64_t> analyze(ImageHitMap *image_hit_map);
  
  private:
    std::queue<AnalyzerOp> read_op_queue;
    std::queue<AnalyzerOp> write_op_queue;
    std::list<ImageHitMap> history_list;
    int INTERVAL;
    ImageCtx *ictx;
  };

  // Analyzer

  // AnalyzerOp

  class AnalyzerOp
  {
  public:
    AnalyzerOp(time_t time, uint64_t off, uint64_t len);
    uint64_t get_off();

  private:
    time_t time;
    uint64_t off;
    uint64_t len;
  };

  // AnalyzerOp

  // ImageHitMap

  class ImageHitMap
  {
  public:
    ImageHitMap(std::map<uint64_t, uint64_t> read_map, std::map<uint64_t, uint64_t> write_map, ExtentMap extent_map);
    void print_map();
    uint64_t get_read_ios();
    uint64_t get_write_ios();
    uint64_t get_hdd_extent_num();
    uint64_t get_ssd_extent_num();
    void set_extent_pool(uint64_t extent_id, int pool);

  private:
    std::map<uint64_t, uint64_t> read_map;
    std::map<uint64_t, uint64_t> write_map;
    ExtentMap extent_map; 
  };

  // ImageHitMap
  
  struct ImageCtx {
    // Extent
    struct ExtentMap extent_map;

    int init_extentmap();
    void start_analyzer();
    int get_pool_decision(uint64_t off, size_t len);
    uint64_t get_extent_id(uint64_t off);
    // Extent

    Analyzer *analyzer;

    // Migrater
    
    void do_migrate(uint64_t extent_id, int from_pool, int to_pool);
    object_t map_object(uint64_t off);

    // Migrater

    CephContext *cct;
    PerfCounters *perfcounter;
    struct rbd_obj_header_ondisk header;
    ::SnapContext snapc;
    std::vector<librados::snap_t> snaps; // this mirrors snapc.snaps, but is in
                                        // a format librados can understand
    std::map<std::string, SnapInfo> snaps_by_name;
    uint64_t snap_id;
    bool snap_exists; // false if our snap_id was deleted
    // whether the image was opened read-only. cannot be changed after opening
    bool read_only;
    bool flush_encountered;

    std::map<rados::cls::lock::locker_id_t,
	     rados::cls::lock::locker_info_t> lockers;
    bool exclusive_locked;
    std::string lock_tag;

    std::string name;
    std::string snap_name;
    IoCtx data_ctx[2];
    IoCtx md_ctx[2];
    WatchCtx *wctx;
    int refresh_seq;    ///< sequence for refresh requests
    int last_refresh;   ///< last completed refresh

    /**
     * Lock ordering:
     * md_lock, cache_lock, snap_lock, parent_lock, refresh_lock
     */
    RWLock md_lock; // protects access to the mutable image metadata that
                   // isn't guarded by other locks below
                   // (size, features, image locks, etc)
    Mutex cache_lock; // used as client_lock for the ObjectCacher
    RWLock snap_lock; // protects snapshot-related member variables:
    RWLock parent_lock; // protects parent_md and parent
    Mutex refresh_lock; // protects refresh_seq and last_refresh

    bool old_format;
    uint8_t order;
    uint64_t size;
    uint64_t features;
    std::string object_prefix;
    char *format_string;
    std::string header_oid;
    std::string id; // only used for new-format images
    parent_info parent_md;
    ImageCtx *parent;
    uint64_t stripe_unit, stripe_count;

    ceph_file_layout layout;

    ObjectCacher *object_cacher;
    LibrbdWriteback *writeback_handler;
    ObjectCacher::ObjectSet *object_set;

    /**
     * Either image_name or image_id must be set.
     * If id is not known, pass the empty std::string,
     * and init() will look it up.
     */
    ImageCtx(const std::string &image_name, const std::string &image_id,
	     const char *snap, IoCtx& p0, IoCtx& p1, bool read_only);
    ~ImageCtx();
    int init();
    void init_layout();
    void perf_start(std::string name);
    void perf_stop();
    int get_read_flags(librados::snap_t snap_id);
    int snap_set(std::string in_snap_name);
    void snap_unset();
    librados::snap_t get_snap_id(std::string in_snap_name) const;
    int get_snap_name(snapid_t snap_id, std::string *out_snap_name) const;
    int get_parent_spec(snapid_t snap_id, parent_spec *pspec);
    int is_snap_protected(string in_snap_name, bool *is_protected) const;
    int is_snap_unprotected(string in_snap_name, bool *is_unprotected) const;

    uint64_t get_current_size() const;
    uint64_t get_object_size() const;
    string get_object_name(uint64_t num) const;
    uint64_t get_num_objects() const;
    uint64_t get_stripe_unit() const;
    uint64_t get_stripe_count() const;
    uint64_t get_stripe_period() const;

    void add_snap(std::string in_snap_name, librados::snap_t id,
		  uint64_t in_size, uint64_t features,
		  parent_info parent, uint8_t protection_status);
    uint64_t get_image_size(librados::snap_t in_snap_id) const;
    int get_features(librados::snap_t in_snap_id,
		     uint64_t *out_features) const;
    int64_t get_parent_pool_id(librados::snap_t in_snap_id) const;
    std::string get_parent_image_id(librados::snap_t in_snap_id) const;
    uint64_t get_parent_snap_id(librados::snap_t in_snap_id) const;
    int get_parent_overlap(librados::snap_t in_snap_id,
			   uint64_t *overlap) const;
    void aio_read_from_cache(object_t o, bufferlist *bl, size_t len,
			     uint64_t off, Context *onfinish);
    void write_to_cache(object_t o, bufferlist& bl, size_t len, uint64_t off,
			Context *onfinish);
    int read_from_cache(object_t o, bufferlist *bl, size_t len, uint64_t off);
    void user_flushed();
    void flush_cache_aio(Context *onfinish);
    int flush_cache();
    void shutdown_cache();
    void invalidate_cache();
    void clear_nonexistence_cache();
    int register_watch();
    void unregister_watch();
    size_t parent_io_len(uint64_t offset, size_t length,
			 librados::snap_t in_snap_id);
    uint64_t prune_parent_extents(vector<pair<uint64_t,uint64_t> >& objectx,
				  uint64_t overlap);

  };
}

#endif
