#include "Analyzer.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <pthread.h>
#include <unistd.h>

#include "thrift/storage_service/StorageService.h"

using namespace std;
using namespace monitor;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

Analyzer::Analyzer()
{
	m_analyzing = false;
}

void* analyzing(void* argv)
{
	Analyzer* analyzer = (Analyzer*)argv;
  boost::shared_ptr<TTransport> socket(new TSocket("10.0.0.20", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	storage::StorageServiceClient client(protocol); 
	while(analyzer->m_analyzing)
	{
		try
		{
			transport->open();
			client.do_migration("aa", 0, 1);
			transport->close();
		}
		catch(TException& tx)
		{
			cout << "ERROR: " << tx.what() << endl;
		}
		sleep(10);
	}
}

void Analyzer::start()
{
	pthread_t pid;
	m_analyzing = true;
	pthread_create(&pid, NULL, analyzing, this);
}

void Analyzer::stop()
{
	m_analyzing = false;
}
