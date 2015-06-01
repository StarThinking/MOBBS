#ifndef MOBBS_UTIL_H
#define MOBBS_UTIL_H

#include <string>
#include <vector>

namespace MobbsUtil
{
	int extent2Monitor(const std::string& eid, int base);
	void takeLog(const std::string& filename, const std::string content);
}

#endif
