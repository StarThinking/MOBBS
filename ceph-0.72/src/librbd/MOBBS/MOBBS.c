#include "librbd/MOBBS/MOBBS.h"

int DEFAULT_POOL = HDD_POOL;
int TAKE_LOG_LIBRBD = 0;
int TAKE_LOG_ANALYZER = 0;
int TAKE_LOG_MIGRATER = 0;
int EXTENT_MAP_REBUILD = 0;
int DO_MIGRATION = 0;

void take_log(const char* log)
{
	FILE* file = fopen("/etc/ceph/mobbs.log", "a");
	fprintf(file, "%s\n", log);
	fclose(file);
}
