#include "renderer/resource/CBasicMaterial.h"

#include "renderer/resource/CShader.h"
#include "renderer/resource/CTexture.h"

CBasicMaterial::CBasicMaterial(CShader* shader, float ambient, float specular, CTexture* texture)
	:
	AMaterial(shader),
	d_ambient(ambient),
	d_specular(specular),
	d_texture(texture)
{
	d_shader->setActive();
	d_shader->setShaderValueFloat("ambient", d_ambient);
	d_shader->setShaderValueFloat("specular", d_specular);
}

void CBasicMaterial::setActive() const
{
	d_shader->setActive();
	// Set texture to unit 0
	d_texture->setActive(0);
	// Set sampler to texture unit 0
	d_shader->setShaderTexture("texture0", 0);
	return;
}
