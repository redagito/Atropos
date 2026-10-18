#pragma once

#include <glad/gl.h>

#include <cstddef>

class FragmentShader
{
public:
	FragmentShader(const char** shaderCode, std::size_t shaderLineCount);
	~FragmentShader();

	FragmentShader(const FragmentShader& fs) = delete;
	FragmentShader& operator=(const FragmentShader& fs) = delete;

	GLuint getId() const;
private:
	GLuint d_shaderId;
};