#pragma once

#include <glad/glad.h>

/*
* \brief Represents a compiled vertex shader
*/
class VertexShader
{
public:
	VertexShader(const char** shaderCode, int shaderLineCount);
	~VertexShader();
	GLuint getId() const;
private:
	// Disable copying
	VertexShader(const VertexShader& vs);
	GLuint d_shaderId;
};