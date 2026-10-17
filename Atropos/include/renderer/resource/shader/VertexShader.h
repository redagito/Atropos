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

	// Disable copying
	VertexShader(const VertexShader& vs) = delete;
	VertexShader& operator=(const VertexShader& vs) = delete;

	GLuint getId() const;
private:
	GLuint d_shaderId;
};