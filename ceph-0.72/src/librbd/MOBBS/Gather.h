#ifndef GATHER_H
#define GATHER_H

//#include "thrift/monitor_service/MonitorService.h"
#include "MOBBS.h"
#include "librbd/ImageCtx.h"

#include <pthread.h>

namespace librbd
{
	enum IOType
	{
		IO_READ, IO_WRITE
	};

	class Gather
	{
	public:
		Gather(ImageCtx* ictx);
		~Gather();
		void start();
		void stop();
		void report_to_monitor();
		void increase_io_count(std::string eid, IOType io_type);

		pthread_t m_pid;
		bool m_is_gathering;
		void* m_client_info;
		ImageCtx* m_ictx;
		pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
	};
}

#endif
