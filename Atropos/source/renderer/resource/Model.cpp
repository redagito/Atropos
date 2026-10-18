#include "renderer/resource/Model.h"

#include "renderer/resource/AMaterial.h"
#include "renderer/resource/Shader.h"
#include "renderer/resource/Mesh.h"

#include <spdlog/spdlog.h>

Model::Model(Mesh* mesh, AMaterial* material)
	:
	d_mesh(mesh),
	d_material(material)
{
	d_vao.setActive();
	
	GLint vLoc = d_material->getShader()->getAttributeLocation("vertex");
	GLint nLoc = d_material->getShader()->getAttributeLocation("normal");
	GLint uLoc = d_material->getShader()->getAttributeLocation("uv");
	
	if (vLoc != -1)
	{
		glEnableVertexAttribArray(vLoc);
		d_mesh->getVertexBuffer()->setActive();
		glVertexAttribPointer(vLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (nLoc != -1)
	{
		glEnableVertexAttribArray(nLoc);
		d_mesh->getNormalBuffer()->setActive();
		glVertexAttribPointer(nLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (uLoc != -1) 
	{
		glEnableVertexAttribArray(uLoc);
		d_mesh->getUVBuffer()->setActive();
		glVertexAttribPointer(uLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	d_vao.setInactive();
}

Model::~Model()
{

}

const VertexArrayObject* Model::getVertexArrayObject() const
{
	return &d_vao;
}

const Mesh* Model::getMesh() const
{
	return d_mesh;
}

const AMaterial* Model::getMaterial() const
{
	return d_material;
}

