#include "Analyzer.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <pthread.h>
#include <unistd.h>

#include "thrift/storage_service/StorageService.h"
#include "thrift/client_service/ClientService.h"

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
	while(analyzer->m_analyzing)
	{
		map<string, ExtentDetail>::iterator it;
		for(it = analyzer->m_extents.begin(); it != analyzer->m_extents.end(); it ++)
		{
			cout << "start migration " << it->second.m_eid << endl;
			analyzer->apply_migration(it->second.m_eid);
			sleep(10);
		}
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

void Analyzer::apply_migration(string eid)
{
	string client_ip = m_extents[eid].m_client;
  boost::shared_ptr<TTransport> socket(new TSocket(client_ip, 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	librbd::ClientServiceClient client(protocol);
	try
	{
		transport->open();
		int from = m_extents[eid].m_pool;
		int to = (from + 1) % 2;
		client.begin_migration(eid, from, to);
		transport->close();
	}
	catch(TException& tx)
	{
		cout << "Fail to connect client:" << client_ip << endl;
	}
}

void Analyzer::command_migration(string eid)
{
	string storage_ip = m_extents[eid].m_storage;
  boost::shared_ptr<TTransport> socket(new TSocket(storage_ip, 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	storage::StorageServiceClient client(protocol);
	try
	{
		transport->open();
		int from = m_extents[eid].m_pool;
		int to = (from + 1) % 2;
		client.do_migration(eid, from, to);
		transport->close();
	}
	catch(TException& tx)
	{
		cout << "Fail to connect storage:" << storage_ip << endl;
	}
}

void Analyzer::finish_migration(string eid)
{
	string client_ip = m_extents[eid].m_client;
  boost::shared_ptr<TTransport> socket(new TSocket(client_ip, 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

	librbd::ClientServiceClient client(protocol);
	try
	{
		transport->open();
		int from = m_extents[eid].m_pool;
		int to = (from + 1) % 2;
		client.finish_migration(eid, from, to);
		m_extents[eid].m_pool = to;
		transport->close();
	}
	catch(TException& tx)
	{
		cout << "Fail to connect client:" << client_ip << endl;
	}
	
}

string Analyzer::extent_to_osd(string eid)
{
	string result;
	ExtentDetail ed = m_extents[eid];
	string pool_name = ed.m_pool == 0 ? "hdd-pool" : "ssd-pool";
	FILE* fp1, fp2;
	string cmd1 = "ceph osd map " + pool_name + " " + eid;
	fp1 = popen("ceph osd map ", "r");
	return result;
}
