#include "renderer/resource/shader/FragmentShader.h"

#include <stdexcept>
#include <vector>

FragmentShader::FragmentShader(const char** code, std::size_t lines)
{
	d_shaderId = glCreateShader(GL_FRAGMENT_SHADER);

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
		
		std::string err("FragmentShader: Compile error: ");
		err.append(&error[0]);
		glDeleteShader(d_shaderId);
		d_shaderId = 0;
		throw std::runtime_error(err);
	}
}

FragmentShader::~FragmentShader()
{
	glDeleteShader(d_shaderId);
	return;
}

GLuint FragmentShader::getId() const
{
	return d_shaderId;
}
