#include "util/TimeStamp.h"

#include <ctime>
#include <string>
#include <cstdio>
#include <cstring>

std::string TimeStamp::createTimeStamp()
{
	// Create timestamp
	time_t rawtime;
	char buffer[20];
	memset(&buffer[0], 0, 20);

	time(&rawtime);

	struct tm timeinfo;
#ifdef _WIN32
	if (localtime_s(&timeinfo, &rawtime) != 0)
	{
		return std::string();
	}
#else
	if (localtime_r(&rawtime, &timeinfo) == nullptr)
	{
		return std::string();
	}
#endif

	strftime(buffer, 20, "%Y%m%d%H%M%S", &timeinfo);

	// Create log file name
	return std::string(buffer);
}
