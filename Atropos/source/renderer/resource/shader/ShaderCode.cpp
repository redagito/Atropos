#include "renderer/resource/shader/ShaderCode.h"

#include <vector>
#include <cstring>

ShaderCode::ShaderCode(const char* data)
{
	std::string code(data);
	std::vector<std::string> cvec;
	std::string line;
	
	for (unsigned int i = 0; i < code.size(); ++i)
	{
		if (code[i] == '\n')
		{
			line.push_back(code[i]);
			cvec.push_back(line);
			line.clear();
		}
		else if (code[i] == '\r') 
		{
			// ignore these
		}
		else
		{
			line.push_back(code[i]);
		}
	}
	cvec.push_back(line);
	
	d_code = new char*[cvec.size()];
	
	for (unsigned int i = 0; i < cvec.size(); ++i)
	{
		d_code[i] = new char[cvec[i].size() + 1]; // Null terminated
		memcpy(d_code[i], cvec[i].c_str(), sizeof(char) * (cvec[i].size() + 1));
	}
	d_lines = cvec.size();
	
	return;
}

ShaderCode::~ShaderCode()
{
	for (unsigned int i = 0; i < d_lines; ++i)
	{
		delete[] (d_code[i]);
	}
	delete[] d_code;
}

const char** ShaderCode::getCode() const
{
	// TODO drop the cast without needing -fpermissive.
	return (const char**) d_code;
}

std::size_t ShaderCode::getLineCount() const
{
	return d_lines;
}