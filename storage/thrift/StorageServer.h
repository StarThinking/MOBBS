#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H

#include "../MOBBS.h"
#include "../Migrater.h"
#include "storage_service/StorageService.h"

#include <pthread.h>
#include <string>

namespace storage
{
	class StorageServer
	{
	public:
		StorageServer(Migrater* migrater);
		void start();
		void stop();

		Migrater* m_migrater;
		void* m_server;
	};
}

#endif
