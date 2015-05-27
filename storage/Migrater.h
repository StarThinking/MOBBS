#ifndef MIGRATER_H
#define MIGRATER_H

#include <string>
#include <vector>

using namespace std;

class Migrater
{
public:
	Migrater();
	void do_migration(string eid, int from, int to);
	void finish_migration(string eid);
	vector<string> m_monitors;
};

#endif
