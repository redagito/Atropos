#pragma once

#include "renderer/flext/flextGL.h"

/*
* \brief Represents a compiled vertex shader
*/
class CVertexShader
{
public:
	CVertexShader(const char** shaderCode, int shaderLineCount);
	~CVertexShader();
	GLuint getId() const;
private:
	// Disable copying
	CVertexShader(const CVertexShader& vs);
	GLuint d_shaderId;
};