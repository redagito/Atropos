#include "AMaterial.h"

AMaterial::AMaterial(CShader* shader)
	: d_shader(shader)
{
	// Nothing here
}

AMaterial::~AMaterial()
{
	// Nothing to clean up
}

const CShader* AMaterial::getShader() const
{
	return d_shader;
}