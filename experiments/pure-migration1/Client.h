#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using namespace std;

namespace experiment
{
	class Client
	{
	public:
		Client();
		~Client();
		void apply_migration(string eid, int from, int to, string ip);
	};

}

#endif
