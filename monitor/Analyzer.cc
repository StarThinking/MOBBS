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
	int ssd_size = 1024;
	int ssd_cur_size = 0;
	int max_migration_size = 10;
	while(analyzer->m_analyzing)
	{
		cout << "begin analyze" << endl; 
		cout << "current migration queue size: " << analyzer->m_migration_set.size() << endl;
		if(analyzer->m_migration_set.size() > 0)
		{
			cout << "Waiting for last migrations" << endl;
			sleep(5);
			continue;
		}

		ExtentQueue ssd_queue, hdd_queue;
		pthread_mutex_lock(&analyzer->m_extents_lock);
		map<string, ExtentDetail>::iterator it;
		for(it = analyzer->m_extents.begin(); it != analyzer->m_extents.end(); it ++)
		{
			//cout << it->second.m_eid << "  weight: " << it->second.m_weight << endl;
			if(it->second.m_pool == 0) hdd_queue.insert(&it->second);
			else ssd_queue.insert(&it->second);
		}

		ssd_cur_size = ssd_queue.m_length;
		cout << "hdd_queue: " << hdd_queue.m_length << "  ssd_queue: " << ssd_queue.m_length << "  ssd_size: " << ssd_size << endl;

		int hdd_to_ssd = 0;
		int ssd_to_hdd = 0;
		ExtentNode* hdd_queue_ptr = hdd_queue.m_head;
		ExtentNode* ssd_queue_ptr = ssd_queue.m_tail;
		while(ssd_cur_size > ssd_size)
		{
			if(analyzer->m_migration_set.size() >= max_migration_size) break;
			if(ssd_queue_ptr == NULL) break;
			pthread_mutex_lock(&analyzer->m_migration_lock);
			ExtentDetail* sed = ssd_queue_ptr->m_ed;
			ssd_queue_ptr = ssd_queue_ptr->m_prev;
			set<string>::iterator sit = analyzer->m_migration_set.find(sed->m_eid);
			if(sit == analyzer->m_migration_set.end())
			{
				analyzer->m_migration_set.insert(sed->m_eid);
				analyzer->m_migration_queue.push(sed->m_eid);
				ssd_cur_size --;
				ssd_to_hdd ++;
			}
			pthread_mutex_unlock(&analyzer->m_migration_lock);
		}

		int cloop = 0;
		while(hdd_queue_ptr != NULL)
		{
			if(analyzer->m_migration_set.size() >= max_migration_size) break;

			ExtentDetail* hed = hdd_queue_ptr->m_ed;
			hdd_queue_ptr = hdd_queue_ptr->m_next;
			if(ssd_cur_size < ssd_size)
			{
				pthread_mutex_lock(&analyzer->m_migration_lock);
				set<string>::iterator sit = analyzer->m_migration_set.find(hed->m_eid);
				if(sit == analyzer->m_migration_set.end())
				{
					analyzer->m_migration_set.insert(hed->m_eid);
					analyzer->m_migration_queue.push(hed->m_eid);
					ssd_cur_size ++;
					hdd_to_ssd ++;
				}
				pthread_mutex_unlock(&analyzer->m_migration_lock);
			}
			else
			{
				if(ssd_queue_ptr == NULL) break;
				ExtentDetail* sed = ssd_queue_ptr->m_ed;
				if(hed->m_weight > sed->m_weight)
				{
					pthread_mutex_lock(&analyzer->m_migration_lock);
					set<string>::iterator sit = analyzer->m_migration_set.find(hed->m_eid);
					set<string>::iterator sit2 = analyzer->m_migration_set.find(sed->m_eid);
					if(sit == analyzer->m_migration_set.end() && sit2 == analyzer->m_migration_set.end())
					{	
						ssd_queue_ptr = ssd_queue_ptr->m_prev;
						analyzer->m_migration_set.insert(hed->m_eid);
						analyzer->m_migration_queue.push(hed->m_eid);
						analyzer->m_migration_set.insert(sed->m_eid);
						analyzer->m_migration_queue.push(sed->m_eid);

						hdd_to_ssd ++;
						ssd_to_hdd ++;
					}
					pthread_mutex_unlock(&analyzer->m_migration_lock);
				}
				else
				{
					break;
				}
			}
		}

		map<string, ExtentDetail>::iterator it2;
		for(it2 = analyzer->m_extents.begin(); it2 != analyzer->m_extents.end(); it2 ++)
		{
			it2->second.m_weight /= 10;
			if(it2->second.m_weight < 1) it2->second.m_weight = 0;
		}
		cout << "hdd_to_ssd: " << hdd_to_ssd << "  ssd_to_hdd: " << ssd_to_hdd << endl;
		pthread_mutex_unlock(&analyzer->m_extents_lock);
		sleep(10);
	}
}

void* dispatching(void* argv)
{
	Analyzer* analyzer = (Analyzer*)argv;
	while(analyzer->m_analyzing)
	{
		while(!analyzer->m_migration_queue.empty())
		{
			pthread_mutex_lock(&analyzer->m_migration_lock);
			string eid = analyzer->m_migration_queue.front();
			analyzer->m_migration_queue.pop();
			pthread_mutex_unlock(&analyzer->m_migration_lock);
			if(analyzer->m_extents[eid].m_storage == "")
			{
				analyzer->m_extents[eid].m_storage = analyzer->extent_to_osd(eid, analyzer->m_extents[eid].m_pool);
			}
			//cout << "start migration " << eid << endl;
			analyzer->apply_migration(eid);
		}
	}
}

void Analyzer::start()
{
	pthread_t pid;
	m_analyzing = true;
	pthread_create(&pid, NULL, analyzing, this);
	pthread_t pid1;
	pthread_create(&pid1, NULL, dispatching, this);
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
	//cout << eid << " connect storage:" << storage_ip << endl;
  boost::shared_ptr<TTransport> socket(new TSocket(storage_ip, 9091));
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
		cout << eid << "Fail to connect storage:" << storage_ip << endl;
		//cout << eid << "storage ip: " << storage_ip << "  " << tx.what() << endl;
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

		pthread_mutex_lock(&m_migration_lock);
		m_migration_set.erase(eid);
		pthread_mutex_unlock(&m_migration_lock);

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

ExtentQueue::ExtentQueue()
{
	m_length = 0;
	m_head = NULL;
	m_tail = NULL;
}

ExtentQueue::~ExtentQueue()
{
	ExtentNode* p;
	for(p = m_head; p != NULL; p = p->m_next)
	{
		delete(p);
	}
}

int ExtentQueue::insert(ExtentDetail* ed_ptr)
{
	ExtentNode* p = m_head;
	ExtentNode* t = p;
	int pos = 0;
	if(m_head == NULL)
	{
		m_head = new ExtentNode();
		m_head->m_ed = ed_ptr;
		m_head->m_next = NULL;
		m_head->m_prev = NULL;
		m_tail = m_head;
		m_length ++;
		return pos;
	}
	while(p != NULL)
	{
		if(ed_ptr->m_weight < p->m_ed->m_weight)
		{
			t = p;
			p = p->m_next;
			pos ++;
		}
		else
		{
			if(pos == 0)
			{
				m_head = new ExtentNode();
				m_head->m_ed = ed_ptr;
				m_head->m_next = p;
				m_head->m_prev = NULL;
				p->m_prev = m_head;
				m_length++;
				return pos;
			}
			t->m_next = new ExtentNode();
			t->m_next->m_ed = ed_ptr;
			t->m_next->m_next = p;
			t->m_next->m_prev = t;
			p->m_prev = t->m_next;
			m_length ++;
			return pos;
		}
	}
	t->m_next = new ExtentNode();
	t->m_next->m_ed = ed_ptr;
	t->m_next->m_next = p;
	t->m_next->m_prev = t;
	m_length ++;
	m_tail = t->m_next;
	return pos;
}

ExtentDetail* ExtentQueue::pop_head()
{
	if(m_head == NULL) return NULL;
	ExtentNode* node = m_head;
	m_head = node->m_next;
	ExtentDetail* ed = node->m_ed;
	m_length --;
	if(m_length == 0) m_tail = NULL;
	delete(node);
	return ed;
}

ExtentDetail* ExtentQueue::pop_tail()
{
	if(m_tail == NULL) return NULL;
	ExtentNode* node = m_tail;
	m_tail = node->m_prev;
	ExtentDetail* ed = node->m_ed;
	m_length --;
	if(m_length == 0)
	{
		m_head = NULL;
	}
	delete(node);
	return ed;
}
