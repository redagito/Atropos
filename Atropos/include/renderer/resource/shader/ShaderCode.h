#pragma once

#include <cstddef>
#include <string>

/*
* Class ShaderCode
* Represents a shader source code in a more suitable way
*/

class ShaderCode
{
public:
	// Construct from shader code
	ShaderCode(const char* code);
	~ShaderCode();
	
	// Returns array of null terminated character arrays
	const char** getCode() const;
	// Returns number of code lines
	std::size_t getLineCount() const;
private:
	char** d_code;
	std::size_t d_lines;
};