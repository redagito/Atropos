#pragma once

#include "glm/glm.hpp"

#include <unordered_map>
#include <string>

/**
* \brief Reads and writes config files.
* A config file is an ASCII human readable file with key, value pairs.
*/
class ConfigFile
{
public:
	ConfigFile();

	/** \brief Attempts to read the file; throws std::runtime_error if it cannot be opened or holds a malformed line. */
	void load(const std::string& fileName);

	bool has(const std::string& key) const;

	std::string get(const std::string& key, const std::string& defaultValue) const;

	/** \brief Returns the float value for a key, or the default if missing or unparsable. */
	float get(const std::string& key, float defaultValue) const;

	/** \brief Returns the int value for a key, or the default if missing or unparsable. */
	int get(const std::string& key, int defaultValue) const;

	/** \brief Returns the vec3 for a key, or the default if missing; missing components are zero. */
	glm::vec3 get(const std::string& key, const glm::vec3& defaultValue) const;
	
	void set(const std::string& key, const std::string& value);

private:
	void parseLine(const std::string& line);

	std::unordered_map<std::string, std::string> d_values;
};