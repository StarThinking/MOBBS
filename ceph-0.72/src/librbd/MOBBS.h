#ifndef MOBBS_H
#define MOBBS_H

#include <stdio.h>

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
