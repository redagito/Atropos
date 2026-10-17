#pragma once

#include <string>

/*
* Class CShaderCode
* Represents a shader source code in a more suitable way
*/

class CShaderCode
{
public:
	// Construct from shader code
	CShaderCode(const char* code);
	~CShaderCode();
	
	// Returns array of null terminated character arrays
	const char** getCode() const;
	// Returns number of code lines
	int getLineCount() const;
	const char* getLine(unsigned int) const;
private:
	char** d_code;
	unsigned int d_lines;
};