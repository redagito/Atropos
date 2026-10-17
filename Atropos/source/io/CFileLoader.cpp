#include "io/CFileLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Custom memory manager


CFileLoader::CFileLoader(const std::string&path)
	:	d_path(path)
{
	// Initialization
}

bool CFileLoader::load()
{
	std::ifstream fileStream(d_path, std::ios::in);
	d_content.assign("");
	
	// Read in file
	if (fileStream.is_open())
	{
		std::string line;
		while (::std::getline(fileStream, line))
		{
			d_content.append(line).append("\n");
		}
	}
	else // File not found
	{
		return false;
	}
	fileStream.close();

	return true;
}

// Returns contents of the file
const char* CFileLoader::getContent() const
{
	return d_content.c_str();
}