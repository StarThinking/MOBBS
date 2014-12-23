#include "ThreadPool.h"
#include <iostream>

using namespace MobbsUtil;
using namespace std;

struct CreatingParams
{
	ThreadPool* m_thread_pool;
	func m_fptr;
	void* m_argv;
};

ThreadPool::ThreadPool(int num):m_num(num),m_cnum(0)
{
}

ThreadPool::~ThreadPool()
{
}

void* creating(void* argv)
{
	CreatingParams* cp = (CreatingParams*)argv;
	ThreadPool* tp = cp->m_thread_pool;

	cp->m_fptr(cp->m_argv);

	pthread_mutex_lock(&tp->m_mutex_lock);
	tp->m_cnum --;
	pthread_cond_signal(&tp->m_cond_lock);
	pthread_mutex_unlock(&tp->m_mutex_lock);

	delete(cp);
	return NULL;
}

pthread_t ThreadPool::create_thread(func f_ptr, void* argv)
{
	pthread_mutex_lock(&m_mutex_lock);
	while(m_cnum >= m_num)
	{
		pthread_cond_wait(&m_cond_lock, &m_mutex_lock);
	}
	m_cnum ++;

	CreatingParams* cp = new CreatingParams;
	cp->m_thread_pool = this;
	cp->m_fptr = f_ptr;
	cp->m_argv = argv;
	pthread_t pid;
  pthread_create(&pid, NULL, creating, cp);

	pthread_mutex_unlock(&m_mutex_lock);
	return pid;
}
