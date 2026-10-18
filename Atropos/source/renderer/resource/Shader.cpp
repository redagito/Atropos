#include "renderer/resource/Shader.h"

#include "renderer/resource/shader/FragmentShader.h"
#include "renderer/resource/shader/VertexShader.h"
#include "renderer/resource/shader/ShaderCode.h"

#include <spdlog/spdlog.h>

#include <glm/ext.hpp>

#include <stdexcept>
#include <vector>

GLuint Shader::s_activeId = 0;

Shader::Shader()
	:
	d_programId(0)
{
	return;
}


Shader::~Shader()
{
	SPDLOG_DEBUG("Deleting shader.");
	// Guarded so a shader that never linked can be destroyed without a context.
	if (d_programId != 0)
	{
		glDeleteProgram(d_programId);
	}
	if (s_activeId == d_programId)
	{
		s_activeId = 0;
	}
	return;
}

void Shader::init(const ShaderSources& sources)
{
	SPDLOG_DEBUG("Creating shader program from source.");

	ShaderCode vc(sources.vertex.c_str());
	ShaderCode fc(sources.fragment.c_str());

	SPDLOG_DEBUG("Compiling shader objects.");
	VertexShader vs(vc.getCode(), vc.getLineCount());
	FragmentShader fs(fc.getCode(), fc.getLineCount());

    d_programId = glCreateProgram();

	glAttachShader(d_programId, vs.getId());
	glAttachShader(d_programId, fs.getId());
	
	SPDLOG_DEBUG("Linking shader objects.");
	glLinkProgram(d_programId);

	GLint result;
    glGetProgramiv(d_programId, GL_LINK_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length = 0;
		glGetProgramiv(d_programId, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(static_cast<std::size_t>(length) + 1, '\0');
		glGetProgramInfoLog(d_programId, length, NULL, error.data());

		// Release the failed program before reporting the error.
		glDeleteProgram(d_programId);
		d_programId = 0;

		throw std::runtime_error(std::string("Shader link error: ") + error.data());
    }

	SPDLOG_DEBUG("Caching active uniform locations.");
	GLint uniformCount = 0;
	GLint maxNameLength = 0;
	glGetProgramiv(d_programId, GL_ACTIVE_UNIFORMS, &uniformCount);
	glGetProgramiv(d_programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

	std::vector<char> name(static_cast<std::size_t>(maxNameLength));
	for (GLint i = 0; i < uniformCount; ++i)
	{
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum type = 0;
		glGetActiveUniform(d_programId, i, maxNameLength, &length, &count, &type, name.data());

		// Arrays report as "name[0]"; the location then addresses consecutive elements.
		const std::string key(name.data(), static_cast<std::size_t>(length));
		d_uniforms[key] = glGetUniformLocation(d_programId, key.c_str());
	}
}

void Shader::setActive() const
{
	if (d_programId != s_activeId)
	{
		s_activeId = d_programId;
		glUseProgram(d_programId);
	}
	return;
}

void Shader::setShaderValueMat4(const std::string& name, const glm::mat4& mat) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueMat4(loc, mat);
}

void Shader::setShaderValueMat4(GLint loc, const glm::mat4& mat) const
{
	if (loc != -1)
	{
		glProgramUniformMatrix4fv(d_programId, loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}

void Shader::setShaderValueVec3(const std::string& name, const glm::vec3& value) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueVec3(loc, value);
}

void Shader::setShaderValueVec3(GLint loc, const glm::vec3& value) const
{
	if (loc != -1)
	{
		glProgramUniform3fv(d_programId, loc, 1, glm::value_ptr(value));
	}
}

void Shader::setShaderValueVec2(const std::string& name, const glm::vec2& value) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueVec2(loc, value);
}

void Shader::setShaderValueVec2(GLint loc, const glm::vec2& value) const
{
	if (loc != -1)
	{
		glProgramUniform2fv(d_programId, loc, 1, glm::value_ptr(value));
	}
}

void Shader::setShaderValueFloat(const std::string& name, float value) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueFloat(loc, value);
}

void Shader::setShaderValueFloat(GLint loc, float value) const
{
	if (loc != -1)
	{
		glProgramUniform1f(d_programId, loc, value);
	}
}

void Shader::setShaderTexture(const std::string& name, GLint textureUnit) const
{
	GLint loc = getUniformLocation(name);
	if (loc != -1)
	{
		glProgramUniform1i(d_programId, loc, textureUnit);
	}
}

void Shader::setShaderTexture(GLint loc, GLint textureUnit) const
{
	if (loc != -1)
	{
		glProgramUniform1i(d_programId, loc, textureUnit);
	}
}

GLint Shader::getAttributeLocation(const std::string& attrib) const
{
	return glGetAttribLocation(d_programId, attrib.c_str());
}

GLint Shader::getUniformLocation(const std::string& uniform) const
{
	const auto iter = d_uniforms.find(uniform);
	if (iter == d_uniforms.end())
	{
		return -1;
	}
	return iter->second;
}
