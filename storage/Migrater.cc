#include "Migrater.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <pthread.h>

#include "thrift/monitor_service/MonitorService.h"

using namespace std;
using namespace monitor;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

Migrater::Migrater()
{}

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
}
