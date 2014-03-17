// wtitten by masixiang

#ifndef EXTENT_MAP
#define EXTENT_MAP

#define DEFAULT_POOL HDD_POOL
#define HDD_STRIDE -3
#define SSD_STRIDE 3
#define READ_OP 0
#define WRITE_OP 1
#define HDD_POOL 0
#define SSD_POOL 1
#define EXTENT_SIZE 67108864
#define OBJECT_SIZE 4194304
#define INTERVAL 60
#define LAST_BYTE_LIST_SIZE 16
#define SEQ_DISTANCE 4096
#define SMALL_READ_SIZE 131072
#define SMALL_WRITE_SIZE 65536

#define HDD_CAPACITY 6291456
#define REPLICA 3
#define HDD_READ_IOPS 150
#define HDD_WRITE_IOPS 150
//#define SLACK_FACTOR 2.0

struct ExtentMap {
  uint64_t extent_size;
  uint64_t map_size;
  std::map<uint64_t, int> map;
};    

#endif
