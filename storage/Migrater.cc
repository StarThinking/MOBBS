#include "Migrater.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <pthread.h>
#include <rados/librados.hpp>

#include "thrift/monitor_service/MonitorService.h"

using namespace std;
using namespace monitor;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

Migrater::Migrater()
{}

void Migrater::do_migration(string eid, int from, int to)
{
    struct timeval tv_begin, tv_end;
    //struct timeval tv_begin1, tv_end1;
    long time_used;
    gettimeofday(&tv_begin, NULL);
    int ret = 0;
   
	  librados::Rados cluster;
		uint64_t flags;
		cluster.init2("client.admin", "ceph", flags);
		cluster.conf_read_file("/etc/ceph/ceph.conf");
		cluster.connect();

    librados::IoCtx io_ctx_from;
    librados::IoCtx io_ctx_to;

		if(from == 0) 
		{
			cluster.ioctx_create("hdd-pool", io_ctx_from);
			cluster.ioctx_create("ssd-pool", io_ctx_to);
		}
		else
		{
			cluster.ioctx_create("ssd-pool", io_ctx_from);
			cluster.ioctx_create("hdd-pool", io_ctx_to);
		}

    // reading
		int OBJECT_SIZE = 4194304;
    librados::bufferlist read_buf;
    ret = io_ctx_from.read(eid, read_buf, OBJECT_SIZE, 0);
    if( ret < 0 ) 
    {
    	std::cout << "fail to read" << std::endl;
	exit(ret);
    }

    // writing
    ret = io_ctx_to.write_full(eid, read_buf);
    if( ret < 0 ) 
    {
    	std::cout << "fail to write" << std::endl;
	exit(ret);
    }

    // remove
		/*
    io_ctx_from.remove(extent_id);
    if( ret < 0 ) 
    {
    	std::cout << "fail to remove" << std::endl;
	exit(ret);
    }
		*/
		
		io_ctx_from.close();
		io_ctx_to.close();
		cluster.shutdown();

    gettimeofday(&tv_end, NULL);
    time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec); //us
    time_used /= 1000;

		cout << "total time used: " << time_used << " --- " << eid << endl;
	
}

void Migrater::finish_migration(string eid)
{
	boost::shared_ptr<TTransport> socket(new TSocket("10.0.0.10", 9090));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	monitor::MonitorServiceClient client(protocol);

	try
	{
		transport->open();
		client.finish_migration(eid);
		transport->close();
	}
	catch(TException& tx)
	{
		cout << "Fail to connect monitor" << endl;
	}

	cout << "finish migration: " << eid << endl;
}
