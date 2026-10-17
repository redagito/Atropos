#include "renderer/resource/AMaterial.h"

AMaterial::AMaterial(Shader* shader)
	: d_shader(shader)
{
	// Nothing here
}

AMaterial::~AMaterial()
{
	// Nothing to clean up
}

const Shader* AMaterial::getShader() const
{
	return d_shader;
}