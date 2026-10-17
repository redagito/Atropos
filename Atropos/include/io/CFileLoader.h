#pragma once

#include <string>

/*
* \brief Loads text file from disk.
*/
class CFileLoader
{
public:
	/**
	* \brief Sets file to load.
	*/
	CFileLoader(const std::string& file);

	/**
	* \brief Loads the file content to memory.
	*
	* The functon returns true on success and false on error.
	*/
	bool load();

	/**
	* \brief Returns file content.
	*/
	const char* getContent() const;

private:
	/**
	* \brief Stores file content.
	*/
	std::string d_content;

	/**
	* \brief Stores file path
	*/
	std::string d_path;
};
