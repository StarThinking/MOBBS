#include <string>
#include <stdio.h>

#include "ClusterUtil.h"

using namespace MobbsUtil;
using namespace std;

int MobbsUtil::extent2Monitor(const string& eid, int base)
{
	string src = eid.substr(eid.size() - 2, 2);
	int des;
	sscanf(src.c_str(), "%x", &des);
	return des % base;
}
