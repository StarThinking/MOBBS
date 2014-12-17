#ifndef STORAGE_H
#define STORAGE_H

#include "thrift/StorageServer.h"
#include "MOBBS.h"
#include "Migrater.h"

namespace storage
{

	class Storage
	{
	public:
		Storage();
		~Storage();

		Migrater* m_migrater;
		StorageServer* m_storage_server;
	};

}

#endif
