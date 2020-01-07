#include "CBasicMaterial.h"

#include "CShader.h"
#include "CTexture.h"

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

float CBasicMaterial::getAmbient() const
{
	return d_ambient;
}

float CBasicMaterial::getSpecular() const
{
	return d_ambient;
}

void CBasicMaterial::setAmbient(float ambient)
{
	d_ambient = ambient;
	d_shader->setShaderValueFloat("ambient", d_ambient);
}

void CBasicMaterial::setSpecular(float specular)
{
	d_specular = specular;
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
