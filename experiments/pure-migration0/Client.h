#ifndef CLIENT_H
#define CLIENT_H

#include <string>
using namespace std;

namespace experiment
{
	class Client
	{
	public:
		void all_migration(int from, int to, int object_num);
		long read_one_object(string eid, int from);
	};
}

#endif
