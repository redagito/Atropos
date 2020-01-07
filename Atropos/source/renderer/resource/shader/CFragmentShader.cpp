#include "CFragmentShader.h"

#include "debug/Log.h"

#include <stdexcept>
#include <vector>

// Constructor, compiles shader from code
CFragmentShader::CFragmentShader(const char** code, int lines)
{
	// Create new id
	d_shaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Set source code
	glShaderSource(d_shaderId, lines, code , NULL);

    // Compile the shader
	glCompileShader(d_shaderId);

	// Check the shader
	GLint result;
	glGetShaderiv(d_shaderId, GL_COMPILE_STATUS, &result);

	// Throw exception on error
	if (result != GL_TRUE)
	{
		int length;
		glGetShaderiv(d_shaderId, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetShaderInfoLog(d_shaderId, length, NULL, &error[0]);
		
		// Create error string
		std::string err("CFragmentShader: Compile error: ");
		err.append(&error[0]);
		LOG_ERROR("%s", err.c_str());
		throw std::runtime_error(err);
	}
}

// Destructor
CFragmentShader::~CFragmentShader()
{
	glDeleteShader(d_shaderId);
	return;
}

// Private copy constructor
CFragmentShader::CFragmentShader(const CFragmentShader& fs)
{
	// Never use this
	throw std::runtime_error("CFragmentShader: Never copy CFragmentShader objects");
}

// Returns id
GLuint CFragmentShader::getId() const
{
	return d_shaderId;
}
