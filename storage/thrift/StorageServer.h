#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H

#include "../MOBBS.h"
#include "../Migrater.h"
#include "../mobbs_util/ThreadPool.h"
#include "storage_service/StorageService.h"

#include <pthread.h>
#include <string>
#include <queue>

namespace storage
{
	struct MigratingParams
	{
		Migrater* m_migrater;
		string m_eid;
		int m_from;
		int m_to;
	};

	class StorageServer
	{
	public:
		StorageServer(Migrater* migrater);
		~StorageServer();
		void start();
		void stop();

		Migrater* m_migrater;
		void* m_server;
		queue<MigratingParams*> m_queue;
		bool m_listening;
		pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
		MobbsUtil::ThreadPool* m_thread_pool;
	};
}

#endif
