#include "Storage.h"
#include <unistd.h>

using namespace storage;

Storage::Storage()
{
	m_migrater = new Migrater();
	m_storage_server = new StorageServer(m_migrater);
	m_storage_server->start();
}

Storage::~Storage()
{
	m_storage_server->stop();
	delete(m_storage_server);
	delete(m_migrater);
}

int main(int argc, char** argv)
{
	Storage storage;

	while(1)
	{
		sleep(600);
	}

	return 0;
}
