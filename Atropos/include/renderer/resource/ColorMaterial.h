#pragma once

#include "renderer/resource/AMaterial.h"

#include "glm/glm.hpp"

/**
* \brief Single color material.
*/
class ColorMaterial : public AMaterial
{
public:
	/**
	* \brief Creates material from shader and color.
	*/
	ColorMaterial(Shader* shader, const glm::vec3& color);
	
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