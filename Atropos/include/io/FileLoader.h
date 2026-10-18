#pragma once

#include <string>

class FileLoader
{
public:
	FileLoader(const std::string& file);

	/** \brief Loads the file content to memory; throws std::runtime_error if it cannot be opened. */
	void load();

	const char* getContent() const;

private:
	std::string d_content;

	std::string d_path;
};
