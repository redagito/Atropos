#pragma once

class CShader;

#include "renderer/flext/flextGL.h"
#include <glm/ext.hpp>

/**
* \brief Basic shader class provides a specialized interface to a gpu shader.
* The basic shader provides setter for model, projection and view matrices.
* The basic shader can be used either as a base class for further specialized shaders
* which extend the provided interface or it is directly used for simple shading or for
* example shadow mapping.
*/
class CBasicShader
{
public:
	/**
	* \brief Sets the gpu shader resource.
	*/
	CBasicShader(CShader* shader);
	
	/**
	* \brief Initializes uniform locations and returns result.
	* The initialized uniform names are:
	* "model" mat4 model matrix
	* "view" mat4 view matrix
	* "proj" mat4 projection matrix
	*/
	bool init();
	
	/**
	* \brief Sets the mat4 uniform "model" to the specified value.
	*/
	void setModel(const glm::mat4& model);
	
	/**
	* \brief Sets the mat4 uniform "view" to the specified value.
	*/
	void setView(const glm::mat4& view);
	
	/**
	* \brief Sets the mat4 uniform "proj" to the specified value.
	*/
	void setProjection(const glm::mat4& proj);
	
protected:
	CShader* getShader();
	
private:
	GLuint d_modelUniform; /**< Model matrix uniform location. */
	GLuint d_viewUniform; /**< View matrix uniform location. */
	GLuint d_projUniform; /**< Projection matrix uniform location. */
	CShader* d_shader; /**< Shader resource. */
};