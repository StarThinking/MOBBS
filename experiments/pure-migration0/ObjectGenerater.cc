#include "ObjectGenerater.h"
#include <iostream>
#include <stdio.h>
#include <rados/librados.hpp>

using namespace std;
using namespace experiment;

void ObjectGenerater::generate(string pool_name, int object_num)
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

	librados::IoCtx io_ctx;
	ret = rados.ioctx_create(pool_name.c_str(), io_ctx);
	if(ret < 0)
	{
		cout << "create ioctx failed" << endl;
		exit(1);
	}

	cout << "begin generate " << object_num << "objects to " << pool_name << endl;
	int last = -1;
	for(int i = 0; i < object_num; i++)
	{
		char object_name[100];
		sprintf(object_name, "auto-generated-%d", i);
		int object_size = 4 * 1024 * 1024;
		char content[object_size];
		memset(content, 'a', sizeof(char) * object_size);
		content[object_size] = '\0';
		//sprintf(content, "This object is the %dth generated one", i);
		librados::bufferlist bl;
		bl.append(content);
		ret = io_ctx.write_full(object_name, bl);
		if(ret < 0)
		{
			cout << "write object failed" << endl;
			exit(1);
		}

		int cur = i * 100 / object_num;
		if(cur > last)
		{
			last = cur;
			cout << "finished " << cur << "%" << endl;
		}
	}

	rados.shutdown();
	cout << "generate done" << endl;
}
