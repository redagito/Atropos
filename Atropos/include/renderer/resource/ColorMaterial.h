#pragma once

#include "renderer/resource/AMaterial.h"

#include "glm/glm.hpp"

class ColorMaterial : public AMaterial
{
public:
	ColorMaterial(Shader* shader, const glm::vec3& color);
	
	void setActive() const;

private:
	glm::vec3 d_color;
};