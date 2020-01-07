#include "CShader.h"

#include "shader/CFragmentShader.h"
#include "shader/CVertexShader.h"
#include "shader/CShaderCode.h"

#include "io/CFileLoader.h"
#include "debug/Log.h"

#include <glm/ext.hpp>

#include <stdexcept>
#include <vector>

GLuint CShader::s_activeId = 0;

CShader::CShader()
	:
	d_programId(0)
{
	return;
}


// Removes the program from graphic memory
CShader::~CShader()
{
	LOG_DEBUG("Deleting shader.");
	glDeleteProgram(d_programId);
	return;
}

bool CShader::init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	LOG_DEBUG("Creating shader from vertex shader %s and fragment shader %s.", vertexShaderPath.c_str(), fragmentShaderPath.c_str());

	// Load vertex shader
	CFileLoader vl(vertexShaderPath);
	if (!vl.load())
	{
		LOG_ERROR("Failed to load vertex shader %s", vertexShaderPath.c_str());
		return false;
	}

	// Load fragment shader
	CFileLoader fl(fragmentShaderPath);
	if (!fl.load())
	{
		LOG_ERROR("Failed to load fragment shader %s", fragmentShaderPath.c_str());
		return false;
	}

	// Convert to code
	CShaderCode vc(vl.getContent());
	CShaderCode fc(fl.getContent());

	// Compile
	// TODO Remove exceptions in vertex and fragment shader classes and replace with better error handling.
	LOG_DEBUG("Compiling shader objects.");
	CVertexShader vs(vc.getCode(), vc.getLineCount());
	CFragmentShader fs(fc.getCode(), fc.getLineCount());

	// Create a new program id
    d_programId = glCreateProgram();

	// Add the shaders
	glAttachShader(d_programId, vs.getId());
	glAttachShader(d_programId, fs.getId());
	
	// Link the program
	LOG_DEBUG("Linking shader objects.");
	glLinkProgram(d_programId);

    // Check the program
	GLint result;
    glGetProgramiv(d_programId, GL_LINK_STATUS, &result);

	// Check result
	if (result == GL_FALSE)
	{
		int length;
		glGetProgramiv(d_programId, GL_INFO_LOG_LENGTH, &length);
		char* error = new char[length];
		glGetProgramInfoLog(d_programId, length, NULL, error);
		
		std::string err("Shader link error: ");
		err.append(error, length);
		LOG_ERROR("%s", err.c_str());
		delete[] error;
		return false;
    }
	return true;
}

// Set the shader program active
void CShader::setActive() const
{
	if (d_programId != s_activeId)
	{
		s_activeId = d_programId;
		glUseProgram(d_programId);
	}
	return;
}

// Returns id
GLuint CShader::getId() const
{
	return d_programId;
}

// Set 4x4 matrix uniform
void CShader::setShaderValueMat4(const std::string& name, const glm::mat4& mat) const
{
	GLuint loc = getUniformLocation(name);
	setShaderValueMat4(loc, mat);
}

// Set 4x4 matrix uniform
void CShader::setShaderValueMat4(GLuint loc, const glm::mat4& mat) const
{
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}

// Set 3D vector uniform
void CShader::setShaderValueVec3(const std::string& name, const glm::vec3& value) const
{
	GLuint loc = getUniformLocation(name);
	setShaderValueVec3(loc, value);
}

// Set 3D vector uniform
void CShader::setShaderValueVec3(GLuint loc, const glm::vec3& value) const
{
	if (loc != -1)
	{
		glUniform3fv(loc, 1, glm::value_ptr(value));
	}
}

// Set float uniform
void CShader::setShaderValueFloat(const std::string& name, float value) const
{
	GLuint loc = getUniformLocation(name);
	setShaderValueFloat(loc, value);
}

// Set float uniform
void CShader::setShaderValueFloat(GLuint loc, float value) const
{
	if (loc != -1)
	{
		glUniform1f(loc, value);
	}
}

// Set shader texture, needs reworking
void CShader::setShaderTexture(const std::string& name, GLint textureUnit) const
{
	GLuint loc = getUniformLocation(name);
	if (loc != -1)
	{
		glUniform1i(loc, textureUnit);
	}
}

// Set shader texture, needs reworking
void CShader::setShaderTexture(GLuint loc, GLint textureUnit) const
{
	if (loc != -1)
	{
		glUniform1i(loc, textureUnit);
	}
}

// Returns attribute location
GLuint CShader::getAttributeLocation(const std::string& attrib) const
{
	return glGetAttribLocation(d_programId, attrib.c_str());
}

// Returns attribute location
GLuint CShader::getUniformLocation(const std::string& uniform) const
{
	auto iter = d_uniforms.find(uniform);
	if (iter == d_uniforms.end())
	{
		GLuint id = glGetUniformLocation(d_programId, uniform.c_str());
		d_uniforms[uniform] = id;
		return id;
	}
	return iter->second;
}
