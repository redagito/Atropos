#include "CColorMaterial.h"

#include "CShader.h"
#include "CTexture.h"

CColorMaterial::CColorMaterial(CShader* shader, const glm::vec3& color)
	:
	AMaterial(shader),
	d_color(color)
{
	d_shader->setActive();
	d_shader->setShaderValueVec3("color", color);
}

void CColorMaterial::setActive() const
{
	d_shader->setActive();
	d_shader->setShaderValueVec3("color", d_color);
	return;
}
