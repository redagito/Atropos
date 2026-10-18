#include "renderer/resource/AMaterial.h"

AMaterial::AMaterial(Shader* shader)
	: d_shader(shader)
{
}

AMaterial::~AMaterial()
{
}

const Shader* AMaterial::getShader() const
{
	return d_shader;
}