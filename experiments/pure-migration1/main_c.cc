#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include "Client.h"

using namespace experiment;

void* apply_storage0(void* argv)
{
	Client* client = (Client*)argv;
	for(int i = 0; i < 4096; i ++)
	{
		char eid[100];
		sprintf(eid, "auto-generated-%d", i);
		client->apply_migration(eid, 1, 0, "10.0.0.20");
	}
	return NULL;
}

void* apply_storage1(void* argv)
{
	Client* client = (Client*)argv;
	for(int i = 4096; i < 8192; i ++)
	{
		char eid[100];
		sprintf(eid, "auto-generated-%d", i);
		client->apply_migration(eid, 1, 0, "10.0.0.21");
	}
	return NULL;
}


int main(int argc, char** argv)
{
	struct timeval tv_begin, tv_end;
	long time_used;
	gettimeofday(&tv_begin, NULL);

	Client client;
	pthread_t pid0;
	pthread_create(&pid0, NULL, apply_storage0, &client);
	pthread_t pid1;
	pthread_create(&pid1, NULL, apply_storage1, &client);

	pthread_join(pid0, NULL);
	pthread_join(pid1, NULL);

	gettimeofday(&tv_end, NULL);
	time_used = time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec);
	time_used /= 1000;
	cout << "total time used: " << time_used << endl;

	return 0;
}
