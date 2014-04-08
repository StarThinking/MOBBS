// wtitten by masixiang
#include <unistd.h>
#define msleep(x) usleep(x*1000)

#ifndef EXTENT_MAP
#define EXTENT_MAP

#define DEFAULT_POOL HDD_POOL
#define HDD_STRIDE -5
#define SSD_STRIDE 5
#define MAX_MIGRATION 50
#define READ_OP 0
#define WRITE_OP 1
#define HDD_POOL 0
#define SSD_POOL 1
#define EXTENT_SIZE 4194304
#define OBJECT_SIZE 4194304
#define INTERVAL 100
#define LAST_BYTE_LIST_SIZE 32
#define SEQ_DISTANCE 524288
#define SMALL_READ_SIZE 131072
#define SMALL_WRITE_SIZE 65536

#define HDD_CAPACITY 6291456
//#define HDD_CAPACITY 12288
#define REPLICA 3
#define HDD_READ_IOPS 150
#define HDD_WRITE_IOPS 150
#define SLACKNESS 1
#define MIGRATING_TO_SSD 999
#define MIGRATING_TO_HDD -999

#define DO_MIGRATION 
#define RESTORE

struct ExtentMap {
  uint64_t extent_size;
  uint64_t map_size;
  std::map<uint64_t, int> map;
};    

#endif
