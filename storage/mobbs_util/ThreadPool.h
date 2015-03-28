#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

namespace MobbsUtil
{
	typedef void* (*func)(void*);

	class ThreadPool
	{
	public:
		ThreadPool(int num);
		~ThreadPool();
		pthread_t create_thread(func f_ptr, void* argv);

		int m_num, m_cnum;
		pthread_mutex_t m_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t m_cond_lock = PTHREAD_COND_INITIALIZER;
	};
}

#endif
