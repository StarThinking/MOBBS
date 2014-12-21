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
		cout << "got lock: " << eid << endl;
		m_analyzer->command_migration(eid);
  }

  void report_client_info(const ClientInfo& ci) {
    // Your implementation goes here
		for(map<string, ExtentInfo>::const_iterator it = ci.m_extents.begin(); it != ci.m_extents.end(); it ++)
		{
			ExtentInfo ei = it->second;
			ExtentDetail ed;
			ed.m_client = ci.m_ip;
			ed.m_storage = "10.0.0.20";
			ed.m_eid = ei.m_eid;
			ed.m_pool = ei.m_pool;
    	m_analyzer->m_extents[ei.m_eid] = ed;
		}
  }

  void finish_migration(const std::string& eid) {
    // Your implementation goes here
    printf("finish_migration\n");
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
