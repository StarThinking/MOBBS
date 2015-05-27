#include <string>
#include <stdio.h>

#include "ClusterUtil.h"

using namespace MobbsUtil;
using namespace std;

string MobbsUtil::extent2Monitor(const string& eid, const vector<string>& monitors)
{
	int base = monitors.size();
	string src = eid.substr(eid.size() - 2, 2);
	int des;
	sscanf(src.c_str(), "%x", &des);
	return monitors[des % base];
}
