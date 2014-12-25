#ifndef MIGRATER_H
#define MIGRATER_H

#include "gen-cpp/MigraterService.h"
#include <string>

using namespace std;

namespace experiment
{
	class Migrater
	{
	public:
		Migrater();
		~Migrater();

		void finish_migration(string eid);
	};
}

#endif
