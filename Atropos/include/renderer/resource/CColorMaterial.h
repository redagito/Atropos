#pragma once

#include "AMaterial.h"

#include "glm/glm.hpp"

/**
* \brief Single color material.
*/
class CColorMaterial : public AMaterial
{
public:
	/**
	* \brief Creates material from shader and color.
	*/
	CColorMaterial(CShader* shader, const glm::vec3& color);
	
	/**
	* \brief Sets uniform values and binds shader.
	*/
	void setActive() const;

private:
	/**
	* \brief Color
	*/
	glm::vec3 d_color;
};