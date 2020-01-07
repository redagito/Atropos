#pragma once

#include <string>
#include <vector>

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
	* \brief Cleanup.
	*/
	~CFileLoader();
	
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

	/** 
	* \brief Returns specific line, starting by 0
	*/
	const char* getLine(int index) const;

	/**
	* \brief Returns file content as 2d array
	*/
	const char** getContentArray() const;

	/**
	* \brief Returns number of lines.
	*/
	int getLineCount() const;

	/**
	* Returns path.
	*/
	const char* getPath() const;

private:
	/**
	* \brief Stores comtent as 2d array, for shader source loading.
	*/
	char** d_contentArray;

	/**
	* \brief Stores file content.
	*/
	std::string d_content;

	/**
	* \brief Stores lines of the file.
	*/
	std::vector<std::string> d_lines;

	/**
	* \brief Stores file path
	*/
	std::string d_path;
};
