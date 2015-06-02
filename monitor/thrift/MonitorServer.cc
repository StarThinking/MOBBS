#include "MonitorServer.h"
#include "monitor_service/MonitorService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <map>
#include <string>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace std;
using namespace monitor;

void* server_listenning(void* argv);

class MonitorServiceHandler : virtual public MonitorServiceIf {
 public:
 Analyzer* m_analyzer;

  MonitorServiceHandler(Analyzer* analyzer):m_analyzer(analyzer) {
    // Your initialization goes here
  }

  void finish_lock(const std::string& eid) {
    // Your implementation goes here
		//cout << "got lock: " << eid << endl;
		m_analyzer->command_migration(eid);
  }

  void report_client_info(const ClientInfo& ci) {
    // Your implementation goes here
		pthread_mutex_lock(&m_analyzer->m_extents_lock);
		for(map<string, ExtentInfo>::const_iterator it = ci.m_extents.begin(); it != ci.m_extents.end(); it ++)
		{
			ExtentInfo ei = it->second;
			//cout << "!!!" << ei.m_eid << endl;
			map<string, ExtentDetail>::iterator eit = m_analyzer->m_extents.find(ei.m_eid);
			if(eit == m_analyzer->m_extents.end())
			{
				ExtentDetail ed;
				ed.m_client = ci.m_ip;
				//ed.m_storage = m_analyzer->extent_to_osd(ei.m_eid, ei.m_pool);
				ed.m_storage = "";
				ed.m_eid = ei.m_eid;
				ed.m_pool = ei.m_pool;
				ed.m_weight += ei.m_rio + ei.m_wio;
    		m_analyzer->m_extents[ei.m_eid] = ed;
			}
			else
			{
				eit->second.m_weight += ei.m_rio + ei.m_wio;
			}
			//cout << eit->second.m_eid << " --- " << eit->second.m_weight << endl;

		}
		pthread_mutex_unlock(&m_analyzer->m_extents_lock);
  }

  void finish_migration(const std::string& eid) {
    // Your implementation goes here
		//cout << "finifsh migration " << eid << endl;
		m_analyzer->finish_migration(eid);
  }

};

MonitorServer::MonitorServer(Analyzer* analyzer):m_analyzer(analyzer)
{}

void MonitorServer::start()
{
  int port = THRIFT_PORT;
  shared_ptr<MonitorServiceHandler> handler(new MonitorServiceHandler(m_analyzer));
  shared_ptr<TProcessor> processor(new MonitorServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  m_server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  pthread_t pid;
  int ret = pthread_create(&pid, NULL, server_listenning, m_server);
  if(ret != 0)
  {
    cout << "failed to start server!" << endl;
  }
}

void* server_listenning(void* argv)
{
	cout << "start monitor server" << endl;
  TSimpleServer* server = (TSimpleServer*)argv;
  server->serve();
  return NULL;
}

void MonitorServer::stop()
{
  ((TSimpleServer*)m_server)->stop();
  delete((TSimpleServer*)m_server);
}
