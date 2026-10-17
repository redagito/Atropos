#pragma once

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
	int getLineCount() const;
private:
	char** d_code;
	unsigned int d_lines;
};