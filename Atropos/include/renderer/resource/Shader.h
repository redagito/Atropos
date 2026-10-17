#pragma once

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <string>
#include <unordered_map>

/**
* \brief A compiled and linked shader program.
* 
* Loads vertex and fragment shader code from file, compiles and links to shader program.
* A model shader is required to have attributes for "vertex" (model vertices), 
* "normal" (surface normals) and "uv" (texture coordinates)
*/
class Shader
{
public:
	/**
	* \brief Default constructor.
	*/
	Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	/**
	* \brief Deletes shader from VRAM
	*/
	~Shader();

	/**
	* \brief Initializes shader resource.
	*
	* Load vertex and fragment shader and compile to shader program.
	* \param vertexShader Vertex shader file path.
	* \param fragmentShader Fragment shader file path.
	* Throws std::runtime_error if the sources cannot be loaded, compiled or linked.
	*/
	void init(const std::string& vertexShader, const std::string& fragmentShader);

	/** 
	* \brief Set as active shader.
	*/
	void setActive() const;

	/**
	* \brief Sets uniform 4x4 matrix value by name.
	*/
	void setShaderValueMat4(const std::string& name, const glm::mat4& mat) const;
	
	/**
	* \brief Sets uniform 4x4 matrix value by id.
	*/
	void setShaderValueMat4(GLint id, const glm::mat4& mat) const;
	
	/**
	* \brief Sets uniform 3D vector value by name.
	*/
	void setShaderValueVec3(const std::string& name, const glm::vec3& value) const;
	
	/**
	* \brief Sets uniform 3D vector value by id.
	*/
	void setShaderValueVec3(GLint id, const glm::vec3& value) const;
	
	/**
	* \brief Sets uniform float value by name.
	*/
	void setShaderValueFloat(const std::string& name, float value) const;
	
	/**
	* \brief Sets uniform float value by id.
	*/
	void setShaderValueFloat(GLint id, float value) const;
	
	/**
	* \brief Sets texture unit by name.
	*/
	void setShaderTexture(const std::string& name, GLint texture) const;
	
	/**
	* \brief Sets texture unit by id.
	*/
	void setShaderTexture(GLint id, GLint texture) const;

	/**
	* \brief Returns location of attribute in shader
	*/
	GLint getAttributeLocation(const std::string& attrib) const;

	/**
	* \brief Returns location of uniform data in shader.
	*/
	GLint getUniformLocation(const std::string& uniform) const;
	
private:
	GLuint d_programId; /**< Shader program id. */
	mutable std::unordered_map<std::string, GLint> d_uniforms; /**< Caches uniform ids. */
	static GLuint s_activeId; /**< Currently bound shader id. */
};