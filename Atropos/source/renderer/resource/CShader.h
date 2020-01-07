#pragma once

#include "renderer/flext/flextGL.h"
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
class CShader
{
public:
	/**
	* \brief Default constructor.
	*/
	CShader();
	
	/**
	* \brief Deletes shader from VRAM
	*/
	~CShader();

	/**
	* \brief Initializes shader resource.
	*
	* Load vertex and fragment shader and compile to shader program.
	* \param vertexShader Vertex shader file path.
	* \param fragmentShader Fragment shader file path.
	*/
	bool init(const std::string& vertexShader, const std::string& fragmentShader);

	/** 
	* \brief Set as active shader.
	*/
	void setActive() const;

	/** 
	* \brief Returns shader id.
	*/
	GLuint getId() const;
	
	/**
	* \brief Sets uniform 4x4 matrix value by name.
	*/
	void setShaderValueMat4(const std::string& name, const glm::mat4& mat) const;
	
	/**
	* \brief Sets uniform 4x4 matrix value by id.
	*/
	void setShaderValueMat4(GLuint id, const glm::mat4& mat) const;
	
	/**
	* \brief Sets uniform 3D vector value by name.
	*/
	void setShaderValueVec3(const std::string& name, const glm::vec3& value) const;
	
	/**
	* \brief Sets uniform 3D vector value by id.
	*/
	void setShaderValueVec3(GLuint id, const glm::vec3& value) const;
	
	/**
	* \brief Sets uniform float value by name.
	*/
	void setShaderValueFloat(const std::string& name, float value) const;
	
	/**
	* \brief Sets uniform float value by id.
	*/
	void setShaderValueFloat(GLuint id, float value) const;
	
	/**
	* \brief Sets texture unit by name.
	*/
	void setShaderTexture(const std::string& name, GLint texture) const;
	
	/**
	* \brief Sets texture unit by id.
	*/
	void setShaderTexture(GLuint id, GLint texture) const;

	/**
	* \brief Returns location of attribute in shader
	*/
	GLuint getAttributeLocation(const std::string& attrib) const;

	/**
	* \brief Returns location of uniform data in shader.
	*/
	GLuint getUniformLocation(const std::string& uniform) const;
	
private:
	GLuint d_programId; /**< Shader program id. */
	mutable std::unordered_map<std::string, GLuint> d_uniforms; /**< Caches uniform ids. */
	static GLuint s_activeId; /**< Currently bound shader id. */
};