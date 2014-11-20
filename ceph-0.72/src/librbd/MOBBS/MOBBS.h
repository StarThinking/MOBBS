#ifndef MOBBS_H
#define MOBBS_H

#include <stdio.h>
#include <unistd.h>

#define OBJECT_SIZE 4194304
//#define DEFAULT_POOL HDD_POOL
#define HDD_POOL 0
#define SSD_POOL 1
#define CONF_PATH "/etc/ceph/mobbs.conf"


// config variable
extern int DEFAULT_POOL;
extern int TAKE_LOG_LIBRBD;
extern int TAKE_LOG_ANALYZER;
extern int TAKE_LOG_MIGRATER;
extern int EXTENT_MAP_REBUILD;
extern int DO_MIGRATION;

// methods
#ifdef __cplusplus
extern "C" {
#endif


// take logs into /etc/ceph/mobbs.log
void take_log(const char* log);

#ifdef __cplusplus
}
#endif

#endif
