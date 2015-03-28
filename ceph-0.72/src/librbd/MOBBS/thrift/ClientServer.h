#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include "librbd/ImageCtx.h"
#include "librbd/MOBBS/MOBBS.h"
#include "librbd/MOBBS/mobbs_util/ThreadPool.h"
#include <pthread.h>
#include <queue>

namespace librbd
{
	struct LockProcessParams
	{
		ImageCtx* m_ictx;
		string m_eid;
		int m_from;
		int m_to;
	};

	class ClientServer
	{
	public:
		ClientServer(ImageCtx* ictx);
		~ClientServer();
		void start();
		void stop();

		ImageCtx* m_ictx;
		pthread_t m_pid;
		void* m_server;
		queue<LockProcessParams*> m_queue;
		bool m_listening;
		pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
		MobbsUtil::ThreadPool* m_thread_pool;
		int m_max_locks;
		int m_cur_locks;
	};
}

#endif
