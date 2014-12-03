#include <rados/librados.hpp>
#include <iostream>
#include <sstream>

#include "Mapper.h"

using namespace std;
using namespace monitor;

Mapper::Mapper():m_cluster_name("ceph"),m_user_name("client.admin"),m_ceph_conf("/etc/ceph/ceph.conf"),
		m_pool_name("hdd-pool"),m_object_name("extent_map")
{
	load_extent_map();
	
}

Mapper::~Mapper()
{
	save_extent_map();
	// destroy lock
	for(map<string, pthread_mutex_t>::iterator it = m_extent_lock_map.begin(); 
		it != m_extent_lock_map.end(); it ++)
	{
		pthread_mutex_destroy(&it->second);
		
	}
}

void Mapper::lock_extent(string eid)
{
	pthread_mutex_lock(&m_lock);
	map<string, pthread_mutex_t>::iterator it = m_extent_lock_map.find(eid);
	if(it == m_extent_lock_map.end())
	{
		pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
		m_extent_lock_map[eid] = lock;
	}
	pthread_mutex_unlock(&m_lock);

	pthread_mutex_lock(&m_extent_lock_map[eid]);
}

void Mapper::release_extent(string eid)
{
	pthread_mutex_unlock(&m_extent_lock_map[eid]);
}

int Mapper::get_pool(string eid)
{
	map<string, int>::iterator it = m_extent_map.find(eid);
	if(it == m_extent_map.end())
	{
		return -1;
	}
	return m_extent_map[eid];
}

void Mapper::set_pool(string eid, int pool)
{
	m_extent_map[eid] = pool;
}

void Mapper::save_extent_map()
{
	const char *pool_name = m_pool_name.c_str();
	int ret;
	librados::Rados rados;
	ret = rados.init("admin");
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't initail rados" << endl;
		rados.shutdown();
		exit(1);
	}
	/*
	const char** argv = {"-c", "/etc/ceph/ceph.conf"};
	ret = randos.conf_parse_argv(2, argv);
	if(ret < 0)
	{
		cerr << "Mapper error: fail to parse config file options" << endl;
		rados.shutdown();
		exit(1);
	}
	*/
	ret = rados.conf_read_file(m_ceph_conf.c_str());
	if(ret < 0)
	{
		cerr << "Mapper error: fail to parse config file" << endl;
		rados.shutdown();
		exit(1);
	}

	ret = rados.connect();
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't connect to cluster" << endl;
		rados.shutdown();
		exit(1);
	}
	
	librados::IoCtx io_ctx;
	ret = rados.ioctx_create(pool_name, io_ctx);
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't set up ioctx" << endl;
		rados.shutdown();
		exit(1);
	}
	librados::bufferlist bl;
	for(map<string, int>::iterator it = m_extent_map.begin(); it != m_extent_map.end(); it ++)
	{
		string key = it->first;
		int value = it->second;
		stringstream ss;
		std::string vs;
		ss << value;
		ss >> vs;
		bl.append(key);
		bl.append("/#/");
		bl.append(vs);
		bl.append("/!/");
	}
	int r = io_ctx.write_full(m_object_name, bl);
	if(ret < 0)
	{
		cerr << "Mapper error: fail to save extent map" << endl;
		rados.shutdown();
		exit(1);
	}

	rados.shutdown();


}

void Mapper::load_extent_map()
{
	const char *pool_name = m_pool_name.c_str();
	int ret;
	librados::Rados rados;
	ret = rados.init("admin");
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't initail rados" << endl;
		rados.shutdown();
		exit(1);
	}
	/*
	const char** argv = {"-c", "/etc/ceph/ceph.conf"};
	ret = randos.conf_parse_argv(2, argv);
	if(ret < 0)
	{
		cerr << "Mapper error: fail to parse config file options" << endl;
		rados.shutdown();
		exit(1);
	}
	*/
	ret = rados.conf_read_file(m_ceph_conf.c_str());
	if(ret < 0)
	{
		cerr << "Mapper error: fail to parse config file" << endl;
		rados.shutdown();
		exit(1);
	}

	ret = rados.connect();
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't connect to cluster" << endl;
		rados.shutdown();
		exit(1);
	}
	
	librados::IoCtx io_ctx;
	ret = rados.ioctx_create(pool_name, io_ctx);
	if(ret < 0)
	{
		cerr << "Mapper error: couldn't set up ioctx" << endl;
		rados.shutdown();
		exit(1);
	}
	
	librados::bufferlist bl;
	int read_len = 4194304;
	ret = io_ctx.read(m_object_name, bl, read_len, 0);
	string s_bl;
	if(ret < 0)
	{
		s_bl = "";
	}
	else
	{
		s_bl = bl.c_str();
	}
	while(1)
	{
		int index1 = s_bl.find("/!/");
		if(index1 < 0) break;
		string key_value = s_bl.substr(0, index1);
		int index2 = key_value.find("/#/");
		string key = key_value.substr(0, index2);
		string value = key_value.substr(index2 + 3);
		int v = atoi(value.c_str());
		m_extent_map.insert(pair<string, int>(key,v));
		s_bl = s_bl.substr(index1 + 3);
	}

	rados.shutdown();

}
