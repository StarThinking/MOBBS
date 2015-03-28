#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <queue>
#include <string>
#include "Client.h"

using namespace experiment;
using namespace std;

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

queue<string> queue0;
queue<string> queue1;

void* apply_storage0_new(void* argv)
{
	Client* client = (Client*)argv;
	while(!queue0.empty())
	{
		string eid = queue0.front();
		queue0.pop();
		client->apply_migration(eid, 0, 1, "10.0.0.20");
	}
	return NULL;
}

void* apply_storage1_new(void* argv)
{
	Client* client = (Client*)argv;
	while(!queue1.empty())
	{
		string eid = queue1.front();
		queue1.pop();
		client->apply_migration(eid, 0, 1, "10.0.0.21");
	}
	return NULL;
}


int main(int argc, char** argv)
{
	Client client;
	for(int i = 0; i < 8192; i ++)
	{
		char eid[100];
		sprintf(eid, "auto-generated-%d", i);
		string ip = client.eid_to_ip(eid, 0);
		cout << eid << ":" << ip << endl;
		if(ip == "10.0.0.20")
		{
			queue0.push(eid);
		}
		else
		{
			queue1.push(eid);
		}
	}
	cout << queue0.size() << " " << queue1.size() << endl;
	
	struct timeval tv_begin, tv_end;
	long time_used;
	gettimeofday(&tv_begin, NULL);

	pthread_t pid0;
	pthread_create(&pid0, NULL, apply_storage0_new, &client);
	pthread_t pid1;
	pthread_create(&pid1, NULL, apply_storage1_new, &client);

	pthread_join(pid0, NULL);
	pthread_join(pid1, NULL);

	gettimeofday(&tv_end, NULL);
	time_used = time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec);
	time_used /= 1000;
	cout << "total time used: " << time_used << endl;

	return 0;
}
