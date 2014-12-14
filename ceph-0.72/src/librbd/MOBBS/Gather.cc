#include "Gather.h"
#include "thrift/monitor_service/MonitorService.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <unistd.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace librbd;

void* gathering(void* argv);

Gather::Gather(ImageCtx* ictx)
{
	m_client_info = new monitor::ClientInfo();
	m_is_gathering = false;
	((monitor::ClientInfo*)m_client_info)->m_ip = "10.0.0.30";
	m_ictx = ictx;
}

Gather::~Gather()
{
	delete((monitor::ClientInfo*)m_client_info);
}

void Gather::start()
{
	m_is_gathering = true;
	pthread_create(&m_pid, NULL, gathering, this);
}

void* gathering(void* argv)
{
	take_log("start gathering client information");

	boost::shared_ptr<TTransport> socket(new TSocket("10.0.0.10", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  monitor::MonitorServiceClient client(protocol);

	Gather* gather = (Gather*)argv;
	while(gather->m_is_gathering)
	{
		client.report_client_info(*((monitor::ClientInfo*)gather->m_client_info));
		((monitor::ClientInfo*)gather->m_client_info)->m_extents.clear();
		sleep(10);
	}

	return NULL;
}

void Gather::stop()
{
	m_is_gathering = false;
}

void Gather::increase_io_count(string eid, IOType io_type)
{
	monitor::ClientInfo* ci = (monitor::ClientInfo*)m_client_info;
	map<string, monitor::ExtentInfo>::iterator it = ci->m_extents.find(eid);
	if(it == ci->m_extents.end())
	{
		monitor::ExtentInfo ei;
		ei.m_eid = eid;
		ei.m_pool = m_ictx->m_mapper->m_extent_map[eid];
		ei.m_rio = 0;
		ei.m_wio = 0;
		ci->m_extents[eid] = ei;
	}
	if(io_type == IO_READ)
	{
		ci->m_extents[eid].m_rio ++;
	}
	else if(io_type == IO_WRITE)
	{
		ci->m_extents[eid].m_wio ++;
	}
}
