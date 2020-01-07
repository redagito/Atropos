#include "CFileLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Custom memory manager


CFileLoader::CFileLoader(const std::string&path)
	:	d_path(path),
		d_contentArray(0)
{
	// Initialization
}

CFileLoader::~CFileLoader()
{
	if (d_contentArray != 0)
	{
		for (unsigned int i = 0; i < d_lines.size(); ++i)
		{
			delete[](d_contentArray[i]);
		}
		delete[] d_contentArray;
		d_contentArray = 0;
	}
}

bool CFileLoader::load()
{
	// Load might be called multiple times, cleanup content array
	if (d_contentArray != 0)
	{
		for (unsigned int i = 0; i < d_lines.size(); ++i)
		{
			delete[] (d_contentArray[i]);
		}
		delete[] d_contentArray;
		d_contentArray = 0;
	}

	std::ifstream fileStream(d_path, std::ios::in);
	d_content.assign("");
	
	// Read in file
	if (fileStream.is_open())
	{
		std::string line;
		while (::std::getline(fileStream, line))
		{
			d_lines.push_back(line);
			d_content.append(line).append("\n");
		}
	}
	else // File not found
	{
		return false;
	}
	fileStream.close();

	// Create content array
	d_contentArray = new char*[d_lines.size()];
	for (unsigned int i = 0; i < d_lines.size(); ++i)
	{
		d_contentArray[i] = new char[d_lines[i].size() + 1]; // Null terminated
		memcpy(d_contentArray[i], d_lines[i].c_str(), sizeof(char) * (d_lines[i].size() + 1));
	}
	return true;
}

// Returns contents of the file
const char* CFileLoader::getContent() const
{
	return d_content.c_str();
}

const char* CFileLoader::getPath() const
{
	return d_path.c_str();
}

const char* CFileLoader::getLine(int i) const
{
	return d_lines[i].c_str();
}

int CFileLoader::getLineCount() const
{
	return d_lines.size();
}