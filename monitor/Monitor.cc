#include "Monitor.h"
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace monitor;

Monitor::Monitor()
{
	m_analyzer = new Analyzer();
	m_analyzer->start();
	m_monitor_server = new MonitorServer(m_analyzer);
	m_monitor_server->start();
}

Monitor::~Monitor()
{
	m_monitor_server->stop();
	delete(m_monitor_server);
	delete(m_analyzer);
}

int main(int argc, char** argv)
{
	Monitor* monitor = new Monitor();
	std::cout << "go to sleep" << std::endl;
	while(1)
	{
		sleep(10);
	}

	return 0;
}
