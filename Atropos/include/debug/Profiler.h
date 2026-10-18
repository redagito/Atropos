#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

/**
 * TODO Make thread safe?
 */
class Profiler
{
public:
	Profiler(const std::string& name);
	~Profiler();

	/**
	 * \brief Formats profiler data and returns as string.
	 */
	static std::string toString();

private:
	struct Data
	{
		uint64_t callCount = 0;
		double totalCallTime = 0.f;
		double minCallTime = 9999999.f;
		double maxCallTime = 0.f;
	};

	double d_time;
	std::string d_name;
	static std::unordered_map<std::string, Data> s_profileData;
};
