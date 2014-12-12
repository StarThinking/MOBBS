#ifndef MONITOR_H
#define MONITOR_H

#include "MOBBS.h"
#include "Analyzer.h"
#include "thrift/MonitorServer.h"

namespace monitor
{
	class Monitor
	{
	public:
		Monitor();
		~Monitor();

		Analyzer* m_analyzer;
		MonitorServer* m_monitor_server;
	};
}

#endif
