#ifndef ANALYZER_H
#define ANALYZER_H

#include "MOBBS.h"
#include <map>
#include <string>
#include <queue>
#include <set>
#include <pthread.h>

using namespace std;

namespace monitor
{
	struct ExtentDetail
	{
		string m_eid;
		int m_pool;
		string m_storage;
		string m_client;
		double m_weight;
	};

	struct ExtentNode
	{
		ExtentDetail* m_ed;
		ExtentNode* m_next;
		ExtentNode* m_prev;
	};

	class ExtentQueue
	{
		public:
			ExtentNode* m_head, *m_tail;
			int m_length;

			ExtentQueue();
			~ExtentQueue();
			ExtentDetail* pop_head();
			ExtentDetail* pop_tail();
			int insert(ExtentDetail* ed_ptr);
	};

  class Analyzer
  {
  public:
    map<string, ExtentDetail> m_extents;
		queue<string> m_migration_queue;
		set<string> m_migration_set;
		bool m_analyzing;
		pthread_mutex_t m_extents_lock = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_t m_migration_lock = PTHREAD_MUTEX_INITIALIZER;
    
    Analyzer();
		void start();
		void stop();
		void apply_migration(string eid);
		void command_migration(string eid);
		void finish_migration(string eid);

		string extent_to_osd(string eid, int pool);
  };
}

#endif
