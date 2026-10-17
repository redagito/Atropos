#include "renderer/resource/shader/FragmentShader.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <vector>

// Constructor, compiles shader from code
FragmentShader::FragmentShader(const char** code, std::size_t lines)
{
	// Create new id
	d_shaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Set source code
	glShaderSource(d_shaderId, static_cast<GLsizei>(lines), code , NULL);

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
		if (length < 1)
		{
			length = 1;
		}
		std::vector<char> error(length);
		glGetShaderInfoLog(d_shaderId, length, NULL, &error[0]);
		
		// Create error string
		std::string err("FragmentShader: Compile error: ");
		err.append(&error[0]);
		SPDLOG_ERROR("{}", err.c_str());
		glDeleteShader(d_shaderId);
		d_shaderId = 0;
		throw std::runtime_error(err);
	}
}

// Destructor
FragmentShader::~FragmentShader()
{
	glDeleteShader(d_shaderId);
	return;
}

// Returns id
GLuint FragmentShader::getId() const
{
	return d_shaderId;
}
