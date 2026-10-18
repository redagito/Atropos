#pragma once

#include <glad/gl.h>
#include "glm/glm.hpp"

#include "renderer/resource/ShaderSources.h"

#include <string>
#include <unordered_map>

/**
* \brief Linked shader program. Model shaders must provide the attributes
* "vertex", "normal" and "uv".
*/
class Shader
{
public:
	Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	~Shader();

	/**
	* \brief Throws std::runtime_error if the sources cannot be compiled or linked.
	*/
	void init(const ShaderSources& sources);

	void setActive() const;

	void setShaderValueMat4(const std::string& name, const glm::mat4& mat) const;
	
	void setShaderValueMat4(GLint id, const glm::mat4& mat) const;
	
	void setShaderValueVec3(const std::string& name, const glm::vec3& value) const;
	
	void setShaderValueVec3(GLint id, const glm::vec3& value) const;
	
	void setShaderValueFloat(const std::string& name, float value) const;
	
	void setShaderValueFloat(GLint id, float value) const;
	
	void setShaderTexture(const std::string& name, GLint texture) const;
	
	void setShaderTexture(GLint id, GLint texture) const;

	GLint getAttributeLocation(const std::string& attrib) const;

	GLint getUniformLocation(const std::string& uniform) const;
	
private:
	GLuint d_programId;
	mutable std::unordered_map<std::string, GLint> d_uniforms;
	static GLuint s_activeId;
};