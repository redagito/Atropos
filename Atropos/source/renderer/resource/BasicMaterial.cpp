#include "renderer/resource/BasicMaterial.h"

#include "renderer/resource/Shader.h"
#include "renderer/resource/Texture.h"

BasicMaterial::BasicMaterial(Shader* shader, float ambient, float specular, Texture* texture)
	:
	AMaterial(shader),
	d_texture(texture),
	d_ambient(ambient),
	d_specular(specular)
{
	d_shader->setActive();
	d_shader->setShaderValueFloat("ambient", d_ambient);
	d_shader->setShaderValueFloat("specular", d_specular);
}

void BasicMaterial::setActive() const
{
	d_shader->setActive();
	d_texture->setActive(0);
	// Sampler uniform must stay bound to the unit passed to setActive above.
	d_shader->setShaderTexture("texture0", 0);
	return;
}
