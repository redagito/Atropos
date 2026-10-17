#pragma once

#include <glad/glad.h>

#include <cstddef>

/*
* \brief Represents a compiled vertex shader
*/
class VertexShader
{
public:
	VertexShader(const char** shaderCode, std::size_t shaderLineCount);
	~VertexShader();
	GLuint getId() const;
private:
	// Disable copying
	VertexShader(const VertexShader& vs);
	GLuint d_shaderId;
};