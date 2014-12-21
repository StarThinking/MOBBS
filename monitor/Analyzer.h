#ifndef ANALYZER_H
#define ANALYZER_H

#include "MOBBS.h"
#include <map>
#include <string>

using namespace std;

namespace monitor
{
	struct ExtentDetail
	{
		string m_eid;
		int m_pool;
		string m_storage;
		string m_client;
	};

  class Analyzer
  {
  public:
    map<string, ExtentDetail> m_extents;
		bool m_analyzing;
    
    Analyzer();
		void start();
		void stop();
		void apply_migration(string eid);
		void command_migration(string eid);
		void finish_migration(string eid);

		string extent_to_osd(string eid);
  };
}

#endif
