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
	/**
	* \brief Set file name.
	*/
	ConfigFile();

	/**
	* \brief Attempts to read the file.
	* \param fileName Config file name.
	* Throws std::runtime_error if the file cannot be opened or contains a
	* malformed line.
	*/
	void load(const std::string& fileName);

	/**
	* \brief Checks existence of a key.
	* \return True if the key exists and false if not.
	*/
	bool has(const std::string& key) const;

	/**
	* \brief Returns value to key or, if not found, the default string value.
	*/
	std::string get(const std::string& key, const std::string& defaultValue) const;

	/**
	* \brief Returns float value to key or, if not found, the default float value.
	* If the value found could not be converted to float, the default value is returned.
	*/
	float get(const std::string& key, float defaultValue) const;

	/**
	* \brief Returns int value to key or, if not found, the default int value.
	* If the value found could not be converted to int, the default value is returned.
	*/
	int get(const std::string& key, int defaultValue) const;

	/**
	* \brief Returns vec3 value to key or, if not found, the default vec3 value.
	* If the value found could not be converted to vec3, the default value is returned.
	* Missing components are set to zero.
	*/
	glm::vec3 get(const std::string& key, const glm::vec3& defaultValue) const;
	
	/**
	* \brief Sets the key to value.
	*/
	void set(const std::string& key, const std::string& value);

private:
	/**
	* \brief Parses single line
	*/
	void parseLine(const std::string& line);

	std::unordered_map<std::string, std::string> d_values; /**< Config data. */
};