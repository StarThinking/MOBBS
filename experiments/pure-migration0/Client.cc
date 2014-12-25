#include <iostream>
#include <stdio.h>
#include <rados/librados.hpp>
#include <sys/time.h>
#include "Client.h"

using namespace std;
using namespace experiment;

void Client::all_migration(int from, int to, int object_num)
{
	librados::Rados rados;
	int ret;
	ret = rados.init("admin");
	if(ret < 0)
	{
		cout << "rados init failed" << endl;
		exit(1);
	}
	ret = rados.conf_read_file("/etc/ceph/ceph.conf");
	if(ret < 0)
	{
		cout << "read conf failed" << endl;
		exit(1);
	}
	ret = rados.connect();
	if(ret < 0)
	{
		cout << "rados connect failed" << endl;
		exit(1);
	}

	string pool_name[2] = {"hdd-pool", "ssd-pool"};
	librados::IoCtx io_ctx_from;
	ret = rados.ioctx_create(pool_name[from].c_str(), io_ctx_from);
	if(ret < 0)
	{
		cout << "create ioctx failed" << endl;
		exit(1);
	}
	librados::IoCtx io_ctx_to;
	ret = rados.ioctx_create(pool_name[to].c_str(), io_ctx_to);
	if(ret < 0)
	{
		cout << "create ioctx failed" << endl;
		exit(1);
	}

	struct timeval tv_begin, tv_end;
	long time_used;
	gettimeofday(&tv_begin, NULL);

	for(int i = 0; i < object_num; i++)
	{
		char object_name[100];
		sprintf(object_name, "auto-generated-%d", i);
		int object_size = 4194304;
		cout << "migrating " << object_name << "from: " << from << " to: " << to << endl;
		librados::bufferlist bl;
		ret = io_ctx_from.read(object_name, bl, object_size, 0);
		if(ret < 0)
		{
			cout << "read object failed" << endl;
			exit(1);
		}

		ret = io_ctx_to.write_full(object_name, bl);
		if(ret < 0)
		{
			cout << "write object failed" << endl;
			exit(1);
		}

		cout << object_name << "finished" << endl;

	}

	gettimeofday(&tv_end, NULL);
	time_used = time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec);
	time_used /= 1000;
	cout << "total time used: " << time_used << endl;

	rados.shutdown();
	cout << "migrate done" << endl;
}
