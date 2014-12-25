// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Client.h"
#include "gen-cpp/ClientService.h"
#include "gen-cpp/MigraterService.h"

#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <pthread.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::experiment;
using namespace std;

class ClientServiceHandler : virtual public ClientServiceIf {
 public:
  ClientServiceHandler() {
    // Your initialization goes here
  }

  void finish_migration(const std::string& eid) {
    // Your implementation goes here
    printf("finish_migration\n");
  }

};

void* listening(void* argv) {
  int port = 9090;
  shared_ptr<ClientServiceHandler> handler(new ClientServiceHandler());
  shared_ptr<TProcessor> processor(new ClientServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return NULL;
}

Client::Client()
{
	pthread_t pid;
	pthread_create(&pid, NULL, listening, NULL);
}

Client::~Client()
{}

void Client::apply_migration(string eid, int from, int to, string ip)
{
	boost::shared_ptr<TTransport> socket(new TSocket(ip, 9090));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	MigraterServiceClient client(protocol);

	try
	{
		transport->open();
		client.do_migration(eid, from, to);
		transport->close();
	}
	catch(TException& tx)
	{
		cout << "Fail to connect migrater" << endl;
	}
	
}
