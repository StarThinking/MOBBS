#include "librbd/MOBBS.h"

void take_log(const char* log)
{
	FILE* file = fopen("/etc/ceph/mobbs.log", "a");
	fprintf(file, "%s\n", log);
	fclose(file);
}
