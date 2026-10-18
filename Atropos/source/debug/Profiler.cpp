#include "debug/Profiler.h"

#include <glfw/glfw3.h>

#include <cassert>
#include <sstream>
#include <iostream>

std::unordered_map<std::string, Profiler::Data> Profiler::s_profileData;

Profiler::Profiler(const std::string& name)
	:
	d_time(glfwGetTime()),
	d_name(name)
{
	return;
}

Profiler::~Profiler()
{
	d_time = (glfwGetTime() - d_time) * 1000.0;

	Profiler::Data& data = s_profileData[d_name];

	data.callCount++;
	data.totalCallTime += d_time;
	
	if (data.maxCallTime < d_time)
	{
		data.maxCallTime = d_time;
	}
	if (data.minCallTime > d_time)
	{
		data.minCallTime = d_time;
	}
}

std::string Profiler::toString()
{
	std::stringstream ss;
	ss << "[Profile data]" << std::endl;

	for (const auto& data : s_profileData)
	{
		ss << "Name: " << data.first << std::endl;
		ss << "\tCall count: " << data.second.callCount << std::endl;
		ss << "\tTotal call time: " << data.second.totalCallTime << " [ms]" << std::endl;
		ss << "\tMin call time: " << data.second.minCallTime << " [ms]" << std::endl;
		ss << "\tMax call time: " << data.second.maxCallTime << " [ms]" << std::endl;
		ss << "\tAverage call time: " << data.second.totalCallTime / data.second.callCount << " [ms]" << std::endl;
	}

	return ss.str();
}