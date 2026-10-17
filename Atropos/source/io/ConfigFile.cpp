#include "io/ConfigFile.h"

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
		try
		{
			return std::stof(d_values.at(key));
		}
		catch (const std::exception&)
		{
			// Conversion error
		}
		return defaultValue;
	}
	return defaultValue;
}

int ConfigFile::get(const std::string& key, int defaultValue) const
{
	if (has(key))
	{
		try
		{
			return std::stoi(d_values.at(key));
		}
		catch (const std::exception&)
		{
			// Conversion error
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
	// Comments and empty lines
	if (line.size() == 0)
	{
		return;
	}
	if (line[0] == '#')
	{
		return;
	}

	std::size_t pos = line.find("=");
	if (pos == std::string::npos)
	{
		throw std::runtime_error("Malformed config line: " + line);
	}
	std::string value = line.substr(pos + 1);
	// Strip the carriage return of CRLF line endings.
	if (!value.empty() && value.back() == '\r')
	{
		value.pop_back();
	}
	d_values[line.substr(0, pos)] = value;
}
