#include "CBasicShader.h"

CBasicShader::CBasicShader(CShader* shader)
	:
	d_shader(shader),
	d_modelUniform(0),
	d_viewUniform(0),
	d_projUniform(0)
{
	return;
}

bool CBasicShader::init()
{
	return false;
}

void CBasicShader::setModel(const glm::mat4& model)
{

}

void CBasicShader::setView(const glm::mat4& view)
{

}

void CBasicShader::setProjection(const glm::mat4& proj)
{

}

CShader* CBasicShader::getShader()
{
	return d_shader;
}