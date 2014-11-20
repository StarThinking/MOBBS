#include "librbd/MOBBS/Analyzer.h"

namespace librbd
{
	Analyzer::Analyzer(ImageCtx* ictx, Migrater* migrater):m_ictx(ictx), m_migrater(migrater), m_analysing(false)
	{}

	void Analyzer::start_analyse()
	{
		if(TAKE_LOG_ANALYZER)
			take_log("start analyse------");

		int ret;
		m_analysing = true;
		ret = pthread_create(&m_pid, NULL, do_analyse, this);
		if(ret != 0)
		{
			std::cout << "fail to start analyse" << std::endl;
			exit(ret);
		}
	}

	void Analyzer::stop_analyse()
	{
		m_analysing = false;
		pthread_join(m_pid, NULL);
	}

	void* do_analyse(void* argv)
	{
		Analyzer* analyzer = (Analyzer*)argv;
		while(analyzer->m_analysing)
		{
			for(std::map<std::string, int>::iterator it = analyzer->m_ictx->extent_map.begin(); it != analyzer->m_ictx->extent_map.end(); it ++)
			{
				if(!analyzer->m_analysing) return NULL;
				std::string extent_id = it->first;
				int from_pool = it->second;
				int to_pool;
				if(from_pool == HDD_POOL) 
				{
					to_pool = SSD_POOL;
					analyzer->m_migrater->do_concurrent_migrate(extent_id, from_pool, to_pool);
				}
			}

			sleep(10);

		}

		return NULL;
	}


}
