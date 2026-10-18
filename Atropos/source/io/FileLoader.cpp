#include "io/FileLoader.h"

#include <fstream>
#include <stdexcept>
#include <string>

FileLoader::FileLoader(const std::string& path)
	:	d_path(path)
{
}

void FileLoader::load()
{
	std::ifstream fileStream(d_path, std::ios::in);

	if (!fileStream.is_open())
	{
		throw std::runtime_error("Failed to open file: " + d_path);
	}

	// Cleared only after the file opened, so a failed load keeps the last good content.
	d_content.assign("");

	std::string line;
	while (std::getline(fileStream, line))
	{
		d_content.append(line).append("\n");
	}
}

const char* FileLoader::getContent() const
{
	return d_content.c_str();
}
