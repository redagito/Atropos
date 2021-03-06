#include "CLogger.h"

#include "Log.h"

#include <cstring>
#include <ctime>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <iostream>

std::ofstream CLogger::s_stream;

void CLogger::log(const char* format, ...)
{
	// Text buffer
	char buffer[5000];
	va_list args;
	va_start (args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	// Print to standard output
	std::cout << buffer << std::endl;
	// Print to file
	if (s_stream.is_open())
	{
		s_stream << buffer << std::endl;
	}
}

bool CLogger::initLogFile(const std::string& logFile)
{
	if (!s_stream.is_open())
	{
		s_stream.open(logFile);
		if (!s_stream.is_open())
		{
			LOG_WARNING("Failed to open log file %s.", logFile.c_str());
			return false;
		}
		return true;
	}
	LOG_WARNING("Multiple logger initialization calls are invalid and will be ignored.");
	return false;
}
