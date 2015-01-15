#include "Analyzer.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <pthread.h>
#include <unistd.h>
#include <regex.h>


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
	int count = 0;
	while(analyzer->m_analyzing)
	{
		map<string, ExtentDetail>::iterator it;
		for(it = analyzer->m_extents.begin(); it != analyzer->m_extents.end(); it ++)
		{	
			//if(it->second.m_pool == 0) continue;
			cout << "start migration " << it->second.m_eid << endl;
			analyzer->apply_migration(it->second.m_eid);
			count ++;
			if(count >= 10) return NULL;
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

string Analyzer::extent_to_osd(string eid, int pool)
{
	string pool_name = pool == 0 ? "hdd-pool" : "ssd-pool";
	int reg_ret;
	regmatch_t pmatch[1];
	const size_t nmatch = 1;
	regex_t reg;

	FILE* fp1;
	char cmd1[1024];
	sprintf(cmd1, "ceph osd map %s %s", pool_name.c_str(), eid.c_str());
	fp1 = popen(cmd1, "r");
	char res1[1024];
	fgets(res1, 1024, fp1);
	pclose(fp1);

	const char* pattern1 = "up \\[[0-9]+,";
	regcomp(&reg, pattern1, REG_EXTENDED);
	regexec(&reg, res1, nmatch, pmatch, 0);
	string tmp1(res1);
	string oid = tmp1.substr(pmatch[0].rm_so + 4, pmatch[0].rm_eo - pmatch[0].rm_so - 5);
	regfree(&reg);

	FILE* fp2;
	fp2 = popen("ceph osd dump", "r");
	string res2;
	while(1)
	{
		char buf[1024];
		char* ret = fgets(buf, 1024, fp2);
		if(ret == NULL) break;
		res2 += buf;
	}
	pclose(fp2);

	char pattern2[1024];
	sprintf(pattern2, "osd.%s .*", oid.c_str());
	regcomp(&reg, pattern2, REG_EXTENDED);
	regexec(&reg, res2.c_str(), nmatch, pmatch, 0);
	string tmp2;
	tmp2 = res2.substr(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
	regfree(&reg);

	const char* pattern3 = "([0-9]+\\.){3}[0-9]+";
	regcomp(&reg, pattern3, REG_EXTENDED);
	regexec(&reg, tmp2.c_str(), nmatch, pmatch, 0);
	string ip;
	ip = tmp2.substr(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
	regfree(&reg);

	return ip;
}
