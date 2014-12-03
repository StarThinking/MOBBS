#include "MOBBS.h"
#include "Mapper.h"
#include "thrift/gen-cpp/MonitorServer.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::monitor;

class MonitorServerHandler : virtual public MonitorServerIf {
 public:
  MonitorServerHandler() {
    // Your initialization goes here
  }

  MonitorServerHandler(Mapper *mapper):m_mapper(mapper) {
  
  }

  int32_t lock_and_get_pool(const std::string& eid) {
    // Your implementation goes here
    m_mapper->lock_extent(eid);
    int pool = m_mapper->get_pool(eid);
    if(pool == -1) {
    	m_mapper->set_pool(eid, DEFAULT_POOL);
    	return DEFAULT_POOL;
    }
    return pool;
  }

  void release_lock(const std::string& eid) {
    // Your implementation goes here
    m_mapper->release_extent(eid);
  }

  private:
   Mapper *m_mapper;

};

void* monitor_serve(void* arg)
{
  int port = THRIFT_PORT;
  shared_ptr<MonitorServerHandler> handler(new MonitorServerHandler((Mapper*)arg));
  shared_ptr<TProcessor> processor(new MonitorServerProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  cout << "Monitor server start to listen" << endl;
  server.serve();

}

pthread_t start_monitor_server(Mapper* mapper)
{
	pthread_t pid;
	pthread_create(&pid, NULL, monitor_serve, (void*)mapper);
	return pid;
}

int main(int argc, char **argv) {
  Mapper mapper;
  pthread_t monitor_server_pid = start_monitor_server(&mapper);
  int **ret;
  pthread_join(monitor_server_pid, (void**)ret);
  return 0;
}

