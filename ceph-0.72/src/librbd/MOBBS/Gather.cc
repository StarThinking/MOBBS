#include "Gather.h"
#include "thrift/monitor_service/MonitorService.h"
#include "mobbs_util/ClusterUtil.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <unistd.h>
#include <vector>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace librbd;

void* gathering(void* argv);

Gather::Gather(ImageCtx* ictx)
{
	m_monitors.push_back("10.0.0.10");
	m_monitors.push_back("10.0.0.20");
	m_monitors.push_back("10.0.0.21");

	m_client_info = new vector<monitor::ClientInfo>;
	for(int i = 0; i < (signed)m_monitors.size(); i ++)
	{
		monitor::ClientInfo ci;
		ci.m_ip = "10.0.0.30";
		((vector<monitor::ClientInfo>*)m_client_info)->push_back(ci);
	}
	m_is_gathering = false;
	m_ictx = ictx;
}

Gather::~Gather()
{
	delete((vector<monitor::ClientInfo>*)m_client_info);
}

void Gather::start()
{
	m_is_gathering = true;
	pthread_create(&m_pid, NULL, gathering, this);
}

void* gathering(void* argv)
{
	take_log("start gathering client information");
	
	vector<monitor::MonitorServiceClient> clients;
	vector<boost::shared_ptr<TTransport> > sockets;
	vector<boost::shared_ptr<TTransport> > transports;
	vector<boost::shared_ptr<TProtocol> > protocols;

	Gather* gather = (Gather*)argv;
	int mons = gather->m_monitors.size();

	for(int i = 0; i < mons; i ++)
	{
		boost::shared_ptr<TTransport> socket(new TSocket(gather->m_monitors[i], 9090));
		sockets.push_back(socket);
  	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		transports.push_back(transport);
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		protocols.push_back(protocol);
  	monitor::MonitorServiceClient client(protocol);
		clients.push_back(client);
	}

	while(gather->m_is_gathering)
	{
		try
		{
			pthread_mutex_lock(&gather->m_lock);
			vector<monitor::ClientInfo> cis;
			for(int i = 0; i < mons; i ++)
			{
				monitor::ClientInfo ci((*((vector<monitor::ClientInfo>*)gather->m_client_info))[i]);
				cis.push_back(ci);
				(*((vector<monitor::ClientInfo>*)gather->m_client_info))[i].m_extents.clear();
			}
			pthread_mutex_unlock(&gather->m_lock);

			for(int i = 0; i < mons; i ++)
			{
				transports[i]->open();
				clients[i].report_client_info(cis[i]);
				transports[i]->close();
			}
		}
		catch(TException& tx)
		{
			take_log("Gather failed to connect to monitor");
		}
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
	pthread_mutex_lock(&m_lock);
	int index = MobbsUtil::extent2Monitor(eid, (signed)m_monitors.size());
	monitor::ClientInfo* ci = &(*(vector<monitor::ClientInfo>*)m_client_info)[index];
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
	pthread_mutex_unlock(&m_lock);
}
