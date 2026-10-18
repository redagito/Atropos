#pragma once

#include <glad/gl.h>

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

	// Should not be copied
	FragmentShader(const FragmentShader& fs) = delete;
	FragmentShader& operator=(const FragmentShader& fs) = delete;

	GLuint getId() const;
private:
	GLuint d_shaderId;
};