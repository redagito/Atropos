#include "io/ConfigFile.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <fstream>
#include <sstream>


ConfigFile::ConfigFile()
{
	return;
}

void ConfigFile::load(const std::string& fileName)
{
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		throw std::runtime_error("Failed to open config file: " + fileName);
	}

	// Parse lines with key value data
	std::string line;
	// Drop any values from a previous load so the result reflects this file only.
	d_values.clear();
	while (std::getline(ifs, line))
	{
		parseLine(line);
	}
}

bool ConfigFile::has(const std::string& key) const
{
	return (d_values.count(key) != 0);
}

std::string ConfigFile::get(const std::string& key, const std::string& defaultValue) const
{
	if (has(key))
	{
		return d_values.at(key);
	}
	return defaultValue;
}

float ConfigFile::get(const std::string& key, float defaultValue) const
{
	if (has(key))
	{
		const std::string& value = d_values.at(key);
		try
		{
			// std::stof stops at the first character it cannot parse, so a value
			// such as "800x" would silently become 800. Requiring the whole value
			// to be consumed makes a typo fall back to the default.
			std::size_t consumed = 0;
			const float result = std::stof(value, &consumed);
			if (consumed == value.size())
			{
				return result;
			}
			SPDLOG_WARN("Config value {} of {} is not a number.", key, value);
		}
		catch (const std::exception&)
		{
			SPDLOG_WARN("Config value {} of {} is not a number.", key, value);
		}
		return defaultValue;
	}
	return defaultValue;
}

int ConfigFile::get(const std::string& key, int defaultValue) const
{
	if (has(key))
	{
		const std::string& value = d_values.at(key);
		try
		{
			std::size_t consumed = 0;
			const int result = std::stoi(value, &consumed);
			if (consumed == value.size())
			{
				return result;
			}
			SPDLOG_WARN("Config value {} of {} is not a number.", key, value);
		}
		catch (const std::exception&)
		{
			SPDLOG_WARN("Config value {} of {} is not a number.", key, value);
		}
		return defaultValue;
	}
	return defaultValue;
}

/*
* Parses vec3 value, 3 float values separated by spaces or returns default value.
* Example: position=3.1020 2.05 1.3
*/
glm::vec3 ConfigFile::get(const std::string& key, const glm::vec3& defaultValue) const
{
	if (has(key))
	{
		std::stringstream ss(d_values.at(key));
		glm::vec3 res(0.f);
		if (!(ss >> res.x))
		{
			return defaultValue;
		}
		ss >> res.y;
		ss >> res.z;
		return res;
	}
	return defaultValue;
}

void ConfigFile::set(const std::string& key, const std::string& value)
{
	d_values[key] = value;
}

void ConfigFile::parseLine(const std::string& line)
{
	// Strip the newline characters of CRLF and LF line endings first, so an
	// empty or comment line is recognised the same way on every platform.
	std::string text = line;
	while (!text.empty() && (text.back() == '\r' || text.back() == '\n'))
	{
		text.pop_back();
	}

	// Comments and empty lines, both of which may be indented.
	const std::size_t firstChar = text.find_first_not_of(" \t");
	if (firstChar == std::string::npos)
	{
		return;
	}
	if (text[firstChar] == '#')
	{
		return;
	}

	std::size_t pos = text.find("=");
	if (pos == std::string::npos)
	{
		throw std::runtime_error("Malformed config line: " + text);
	}
	d_values[text.substr(0, pos)] = text.substr(pos + 1);
}
