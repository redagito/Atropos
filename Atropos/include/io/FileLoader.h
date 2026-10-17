#pragma once

#include <string>

/*
* \brief Loads text file from disk.
*/
class FileLoader
{
public:
	/**
	* \brief Sets file to load.
	*/
	FileLoader(const std::string& file);

	/**
	* \brief Loads the file content to memory.
	*
	* Throws std::runtime_error if the file cannot be opened.
	*/
	void load();

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
