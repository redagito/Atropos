#include "renderer/resource/ColorMaterial.h"

#include "renderer/resource/Shader.h"
#include "renderer/resource/Texture.h"

ColorMaterial::ColorMaterial(Shader* shader, const glm::vec3& color)
	:
	AMaterial(shader),
	d_color(color)
{
	d_shader->setActive();
	d_shader->setShaderValueVec3("color", color);
}

void ColorMaterial::setActive() const
{
	d_shader->setActive();
	d_shader->setShaderValueVec3("color", d_color);
	return;
}
