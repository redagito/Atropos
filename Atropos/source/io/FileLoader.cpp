#include "io/FileLoader.h"

#include <fstream>
#include <stdexcept>
#include <string>

FileLoader::FileLoader(const std::string& path)
	:	d_path(path)
{
	// Initialization
}

void FileLoader::load()
{
	std::ifstream fileStream(d_path, std::ios::in);
	d_content.assign("");

	// Read in file
	if (!fileStream.is_open())
	{
		throw std::runtime_error("Failed to open file: " + d_path);
	}

	std::string line;
	while (std::getline(fileStream, line))
	{
		d_content.append(line).append("\n");
	}
}

// Returns contents of the file
const char* FileLoader::getContent() const
{
	return d_content.c_str();
}
