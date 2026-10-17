#pragma once

#include "glm/glm.hpp"

#include <unordered_map>
#include <string>

/**
* \brief Reads and writes config files.
* A config file is an ASCII human readable file with key, value pairs.
*/
class CConfigFile
{
public:
	/**
	* \brief Set file name.
	*/
	CConfigFile();

	/**
	* \brief Attempts to read the file.
	* \parm fileName Config file name.
	* Returns true if the file was loaded successfully or false on error.
	*/
	bool load(const std::string& fileName);

	/**
	* \brief Saves config data to file.
	*/
	bool save(const std::string& fileName) const;

	/**
	* \brief Checks existence of a key.
	* \return True if the key exists and false if not.
	*/
	bool has(const std::string& key) const;

	/**
	* \brief Returns value to key or, if not found, the default string value.
	*/
	const std::string& get(const std::string& key, const std::string& defaultValue) const;

	/**
	* \brief Returns float value to key or, if not found, the default float value.
	* If the value found could not be converted to float, triggers assert in debug and returns
	* default value in release.
	*/
	float get(const std::string& key, float defaultValue) const;

	/**
	* \brief Returns int value to key or, if not found, the default int value.
	* If the value found could not be converted to int, triggers assert in debug and returns
	* default value in release.
	*/
	int get(const std::string& key, int defaultValue) const;

	/**
	* \brief Returns vec3 value to key or, if not found, the default vec3 value.
	* If the value found could not be converted to vec3, triggers assert in debug and returns
	* default value in release.
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
	bool parseLine(const std::string& line);

	static std::string s_emptyString; /**< Return value for failed lookups. */
	std::unordered_map<std::string, std::string> d_values; /**< Config data. */
};