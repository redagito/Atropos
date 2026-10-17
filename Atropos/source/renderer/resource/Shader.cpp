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


// Removes the program from graphic memory
Shader::~Shader()
{
	SPDLOG_DEBUG("Deleting shader.");
	glDeleteProgram(d_programId);
	if (s_activeId == d_programId)
	{
		s_activeId = 0;
	}
	return;
}

void Shader::init(const ShaderSources& sources)
{
	SPDLOG_DEBUG("Creating shader program from source.");

	// Convert to code
	ShaderCode vc(sources.vertex.c_str());
	ShaderCode fc(sources.fragment.c_str());

	// Compile
	SPDLOG_DEBUG("Compiling shader objects.");
	VertexShader vs(vc.getCode(), vc.getLineCount());
	FragmentShader fs(fc.getCode(), fc.getLineCount());

	// Create a new program id
    d_programId = glCreateProgram();

	// Add the shaders
	glAttachShader(d_programId, vs.getId());
	glAttachShader(d_programId, fs.getId());
	
	// Link the program
	SPDLOG_DEBUG("Linking shader objects.");
	glLinkProgram(d_programId);

    // Check the program
	GLint result;
    glGetProgramiv(d_programId, GL_LINK_STATUS, &result);

	// Check result
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
}

// Set the shader program active
void Shader::setActive() const
{
	if (d_programId != s_activeId)
	{
		s_activeId = d_programId;
		glUseProgram(d_programId);
	}
	return;
}

// Set 4x4 matrix uniform
void Shader::setShaderValueMat4(const std::string& name, const glm::mat4& mat) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueMat4(loc, mat);
}

// Set 4x4 matrix uniform
void Shader::setShaderValueMat4(GLint loc, const glm::mat4& mat) const
{
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}

// Set 3D vector uniform
void Shader::setShaderValueVec3(const std::string& name, const glm::vec3& value) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueVec3(loc, value);
}

// Set 3D vector uniform
void Shader::setShaderValueVec3(GLint loc, const glm::vec3& value) const
{
	if (loc != -1)
	{
		glUniform3fv(loc, 1, glm::value_ptr(value));
	}
}

// Set float uniform
void Shader::setShaderValueFloat(const std::string& name, float value) const
{
	GLint loc = getUniformLocation(name);
	setShaderValueFloat(loc, value);
}

// Set float uniform
void Shader::setShaderValueFloat(GLint loc, float value) const
{
	if (loc != -1)
	{
		glUniform1f(loc, value);
	}
}

// Set shader texture, needs reworking
void Shader::setShaderTexture(const std::string& name, GLint textureUnit) const
{
	GLint loc = getUniformLocation(name);
	if (loc != -1)
	{
		glUniform1i(loc, textureUnit);
	}
}

// Set shader texture, needs reworking
void Shader::setShaderTexture(GLint loc, GLint textureUnit) const
{
	if (loc != -1)
	{
		glUniform1i(loc, textureUnit);
	}
}

// Returns attribute location
GLint Shader::getAttributeLocation(const std::string& attrib) const
{
	return glGetAttribLocation(d_programId, attrib.c_str());
}

// Returns attribute location
GLint Shader::getUniformLocation(const std::string& uniform) const
{
	auto iter = d_uniforms.find(uniform);
	if (iter == d_uniforms.end())
	{
		GLint id = glGetUniformLocation(d_programId, uniform.c_str());
		d_uniforms[uniform] = id;
		return id;
	}
	return iter->second;
}
