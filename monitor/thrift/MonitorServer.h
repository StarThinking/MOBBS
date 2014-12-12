#ifndef MONITOR_SERVER_H
#define MONITOR_SERVER_H

#include "../MOBBS.h"
#include "../Analyzer.h"

namespace monitor
{
  class MonitorServer
  {
  public:
    MonitorServer(Analyzer* analyzer);
    void start();
    void stop();

    Analyzer* m_analyzer;
    void* m_server;
  };
}

#endif
