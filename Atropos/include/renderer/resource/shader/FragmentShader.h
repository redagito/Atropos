#pragma once

#include <glad/glad.h>

/*
* Class FragmentShader
* Represents a compiled fragment shader
*/

class FragmentShader
{
public:
	FragmentShader(const char** shaderCode, int shaderLineCount);
	~FragmentShader();
	GLuint getId() const;
private:
	// Should not be copied
	FragmentShader(const FragmentShader& fs);
	GLuint d_shaderId;
};