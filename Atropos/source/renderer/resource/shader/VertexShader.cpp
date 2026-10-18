#include "renderer/resource/shader/VertexShader.h"

#include <stdexcept>
#include <vector>

VertexShader::VertexShader(const char** code, std::size_t lines)
{
	d_shaderId = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(d_shaderId, static_cast<GLsizei>(lines), code , NULL);
	
	glCompileShader(d_shaderId);

	GLint result;
	glGetShaderiv(d_shaderId, GL_COMPILE_STATUS, &result);

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
		
		std::string err("VertexShader: Compile error: ");
		err.append(&error[0]);
		glDeleteShader(d_shaderId);
		d_shaderId = 0;
		throw std::runtime_error(err);
	}
}

VertexShader::~VertexShader()
{
	glDeleteShader(d_shaderId);
	return;
}

GLuint VertexShader::getId() const
{
	return d_shaderId;
}
