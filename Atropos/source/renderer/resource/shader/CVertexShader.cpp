#include "CVertexShader.h"

#include "debug/Log.h"

#include <stdexcept>
#include <vector>

// Constructor, compiles shader from code
CVertexShader::CVertexShader(const char** code, int lines)
{
	// Create new id
	d_shaderId = glCreateShader(GL_VERTEX_SHADER);

    // Set source code
	glShaderSource(d_shaderId, lines, code , NULL);
	
	// Compile the shader
	glCompileShader(d_shaderId);

	// Check the shader
	GLint result;
	glGetShaderiv(d_shaderId, GL_COMPILE_STATUS, &result);

	// Check result and throw exception on error
	if (result != GL_TRUE)
	{
		int length;
		glGetShaderiv(d_shaderId, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetShaderInfoLog(d_shaderId, length, NULL, &error[0]);
		
		// Create error string
		std::string err("CVertexShader: Compile error: ");
		err.append(&error[0]);
		LOG_ERROR("%s", err.c_str());
		throw std::runtime_error(err);
	}
}

// Destructor
CVertexShader::~CVertexShader()
{
	glDeleteShader(d_shaderId);
	return;
}

// Private copy constructor
CVertexShader::CVertexShader(const CVertexShader& vs)
{
	// Blow up if used
	throw std::runtime_error("CVertexShader: Never copy CVertexShader objects");
}

// Returns id
GLuint CVertexShader::getId() const
{
	return d_shaderId;
}
