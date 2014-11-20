#ifndef ANALYZER
#define ANALYZER

#include "pthread.h"
#include "librbd/ImageCtx.h"
#include "librbd/MOBBS/MOBBS.h"
#include "librbd/MOBBS/Migrater.h"

namespace librbd
{
	class Analyzer
	{
	public:
		Analyzer(ImageCtx* ictx, Migrater* migrater);
		void start_analyse();
		void stop_analyse();

		ImageCtx* m_ictx;
		Migrater* m_migrater;
		pthread_t m_pid;
		bool m_analysing;

	private:
	
	};

	void* do_analyse(void* argv);


}


#endif
