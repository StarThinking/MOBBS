#ifndef MOBBS_H
#define MOBBS_H

#include <stdio.h>
#include <unistd.h>

#define DEFAULT_POOL HDD_POOL
#define HDD_POOL 0
#define SSD_POOL 1

#define TAKE_LOG_LIBRBD
#define TAKE_LOG_IMAGECTX


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
