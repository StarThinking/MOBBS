// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Migrater.h"
#include "gen-cpp/MigraterService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <rados/librados.hpp>
#include <sys/time.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::experiment;
using namespace std;

class MigraterServiceHandler : virtual public MigraterServiceIf {
 public:
  MigraterServiceHandler() {
    // Your initialization goes here
  }

  void do_migration(const std::string& eid, const int32_t from, const int32_t to) {
    // Your implementation goes here
		librados::Rados rados;
		int ret;
		ret = rados.init("admin");
		if(ret < 0)
		{
			cout << "rados init failed" << endl;
			exit(1);
		}
		ret = rados.conf_read_file("/etc/ceph/ceph.conf");
		if(ret < 0)
		{
			cout << "read conf failed" << endl;
			exit(1);
		}
		ret = rados.connect();
		if(ret < 0)
		{
			cout << "rados connect failed" << endl;
			exit(1);
		}

		string pool_name[2] = {"hdd-pool", "ssd-pool"};
		librados::IoCtx io_ctx_from;
		ret = rados.ioctx_create(pool_name[from].c_str(), io_ctx_from);
		if(ret < 0)
		{
			cout << "create ioctx failed" << endl;
			exit(1);
		}
		librados::IoCtx io_ctx_to;
		ret = rados.ioctx_create(pool_name[to].c_str(), io_ctx_to);
		if(ret < 0)
		{
			cout << "create ioctx failed" << endl;
			exit(1);
		}

		struct timeval tv_begin, tv_end;
		long time_used;
		gettimeofday(&tv_begin, NULL);

		int object_size = 4194304;
		cout << "migrating " << eid << "from: " << from << " to: " << to << endl;
		librados::bufferlist bl;
		ret = io_ctx_from.read(eid, bl, object_size, 0);
		if(ret < 0)
		{
			cout << "read object failed" << endl;
			exit(1);
		}

		ret = io_ctx_to.write_full(eid, bl);
		if(ret < 0)
		{
			cout << "write object failed" << endl;
			exit(1);
		}

		cout << eid << " finished" << endl;


		gettimeofday(&tv_end, NULL);
		time_used = time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec);
		time_used /= 1000;
		cout << "total time used: " << time_used << endl;

		rados.shutdown();
		cout << "migrate done" << endl;
  }

};

Migrater::Migrater()
{
  int port = 9090;
  shared_ptr<MigraterServiceHandler> handler(new MigraterServiceHandler());
  shared_ptr<TProcessor> processor(new MigraterServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
}

Migrater::~Migrater()
{}

void Migrater::finish_migration(string eid)
{}

