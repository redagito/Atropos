#pragma once

#include <glad/glad.h>

#include <cstddef>

/*
* Class FragmentShader
* Represents a compiled fragment shader
*/

class FragmentShader
{
public:
	FragmentShader(const char** shaderCode, std::size_t shaderLineCount);
	~FragmentShader();
	GLuint getId() const;
private:
	// Should not be copied
	FragmentShader(const FragmentShader& fs);
	GLuint d_shaderId;
};