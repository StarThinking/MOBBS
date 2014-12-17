#ifndef ANALYZER_H
#define ANALYZER_H

#include "MOBBS.h"
#include <map>
#include <string>

using namespace std;

namespace monitor
{
  class Analyzer
  {
  public:
    map<string, int> m_extent_pool;
		bool m_analyzing;
    
    Analyzer();
		void start();
		void stop();
  };
}

#endif
