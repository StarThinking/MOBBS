// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "ClientServer.h"
#include "gen-cpp/ClientService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::librbd;
using namespace std;

class ClientServiceHandler;

struct LockProcessParams
{
	ClientServiceHandler* m_csh_ptr;
	string m_eid;
	int m_from;
	int m_to;
};

void* lock_process(void* argv);
void* listenning(void* argv);

class ClientServiceHandler : virtual public ClientServiceIf {
 public:
  ClientServiceHandler(ImageCtx* ictx):m_ictx(ictx) {
    // Your initialization goes here
  }

  void begin_migration(const std::string& eid, const int32_t from, const int32_t to) {
    // Your implementation goes here
    pthread_t pid;
    LockProcessParams* lpp = new LockProcessParams;
    lpp->m_csh_ptr = this;
    lpp->m_eid = eid;
    lpp->m_from = from;
    lpp->m_to = to;
    pthread_create(&pid, NULL, lock_process, lpp);
  }

  void finish_migration(const std::string& eid, const int32_t from, const int32_t to) {
    // Your implementation goes here
 	m_ictx->m_mapper->set_pool(eid, to);
	m_ictx->m_mapper->release_extent(eid);

	char my_log2[100];
	sprintf(my_log2, "finish migration: eid %s from %d to %d", eid.c_str(), from, to);
	take_log(my_log2);
  }

  ImageCtx* m_ictx;

};

void* lock_process(void* argv)
{
	LockProcessParams* lpp = (LockProcessParams*)argv;
	ClientServiceHandler* csh = lpp->m_csh_ptr;
	ImageCtx* ictx = csh->m_ictx;
	string eid = lpp->m_eid;
	int from = lpp->m_from;
	int to = lpp->m_to;

	char my_log1[100];
	sprintf(my_log1, "migration request: eid %s from %d to %d", eid.c_str(), from, to);
	take_log(my_log1);

	ictx->m_mapper->lock_extent(eid);

	char my_log2[100];
	sprintf(my_log2, "extent locked: eid %s", eid.c_str());
	take_log(my_log2);
	return NULL;
}

ClientServer::ClientServer(ImageCtx* ictx):m_ictx(ictx)
{}

void ClientServer::start()
{
  	int port = THRIFT_PORT;
  	shared_ptr<ClientServiceHandler> handler(new ClientServiceHandler(m_ictx));
  	shared_ptr<TProcessor> processor(new ClientServiceProcessor(handler));
  	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  	m_server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
	int ret = pthread_create(&m_pid, NULL, listenning, this);
	if(ret != 0) 
	{
		std::cout << "fail to start client server" << std::endl;
	}
}

void* listenning(void* argv) {
  ClientServer* cs = (ClientServer*)argv;

  take_log("Client server started");

  ((TSimpleServer*)cs->m_server)->serve();

  take_log("Client server stopped");

  return NULL;
}

void ClientServer::stop()
{
	((TSimpleServer*)m_server)->stop();
  	delete((TSimpleServer*)m_server);
}
