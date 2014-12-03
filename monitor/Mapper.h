#ifndef MAPPER_H
#define MAPPER_H

#include <string>
#include <map>
#include <pthread.h>

#include "MOBBS.h"

using namespace std;

namespace monitor
{
	class Mapper
	{
	public:
		Mapper();
		~Mapper();
		void lock_extent(string eid);
		void release_extent(string eid);
		int get_pool(string eid);
		void set_pool(string eid, int pool);

	private:
		string m_cluster_name;
		string m_user_name;
		string m_ceph_conf;
		string m_pool_name;
		string m_object_name;
		pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
		map<string, int> m_extent_map;
		map<string, pthread_mutex_t> m_extent_lock_map;
		void save_extent_map();
		void load_extent_map();

	};
}

#endif
