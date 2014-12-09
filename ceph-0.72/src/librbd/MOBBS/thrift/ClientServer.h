#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include "librbd/ImageCtx.h"
#include "librbd/MOBBS/MOBBS.h"
#include <pthread.h>

namespace librbd
{
	class ClientServer
	{
	public:
		ClientServer(ImageCtx* ictx);
		void start();
		void stop();

		ImageCtx* m_ictx;
		pthread_t m_pid;
		void* m_server;
	};
}

#endif
