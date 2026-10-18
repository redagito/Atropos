#pragma once

#include <cstddef>
#include <string>

class ShaderCode
{
public:
	ShaderCode(const char* code);

	ShaderCode(const ShaderCode&) = delete;
	ShaderCode& operator=(const ShaderCode&) = delete;

	~ShaderCode();
	
	// Returns array of null terminated character arrays
	const char** getCode() const;
	std::size_t getLineCount() const;
private:
	char** d_code;
	std::size_t d_lines;
};