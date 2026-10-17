#pragma once

#include "renderer/flext/flextGL.h"

/*
* Class CFragmentShader
* Represents a compiled fragment shader
*/

class CFragmentShader
{
public:
	CFragmentShader(const char** shaderCode, int shaderLineCount);
	~CFragmentShader();
	GLuint getId() const;
private:
	// Should not be copied
	CFragmentShader(const CFragmentShader& fs);
	GLuint d_shaderId;
};