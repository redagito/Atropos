#include "renderer/resource/shader/VertexShader.h"

#include "debug/Log.h"

#include <stdexcept>
#include <vector>

// Constructor, compiles shader from code
VertexShader::VertexShader(const char** code, std::size_t lines)
{
	// Create new id
	d_shaderId = glCreateShader(GL_VERTEX_SHADER);

    // Set source code
	glShaderSource(d_shaderId, static_cast<GLsizei>(lines), code , NULL);
	
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
		std::string err("VertexShader: Compile error: ");
		err.append(&error[0]);
		LOG_ERROR("%s", err.c_str());
		throw std::runtime_error(err);
	}
}

// Destructor
VertexShader::~VertexShader()
{
	glDeleteShader(d_shaderId);
	return;
}

// Private copy constructor
VertexShader::VertexShader(const VertexShader&)
{
	// Blow up if used
	throw std::runtime_error("VertexShader: Never copy VertexShader objects");
}

// Returns id
GLuint VertexShader::getId() const
{
	return d_shaderId;
}
