#include "renderer/resource/Model.h"

#include "renderer/resource/AMaterial.h"
#include "renderer/resource/Shader.h"
#include "renderer/resource/Mesh.h"

#include <spdlog/spdlog.h>

namespace
{
	/** \brief Each attribute keeps its own buffer, so each needs its own binding slot. */
	void bindAttribute(GLuint vao, GLint location, GLuint binding, GLuint buffer, GLint components)
	{
		glVertexArrayVertexBuffer(vao, binding, buffer, 0, 0);
		glVertexArrayAttribFormat(vao, static_cast<GLuint>(location), components, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, static_cast<GLuint>(location), binding);
		glEnableVertexArrayAttrib(vao, static_cast<GLuint>(location));
	}
}

Model::Model(Mesh* mesh, AMaterial* material)
	:
	d_mesh(mesh),
	d_material(material)
{
	const GLuint vao = d_vao.getId();

	GLint vLoc = d_material->getShader()->getAttributeLocation("vertex");
	GLint nLoc = d_material->getShader()->getAttributeLocation("normal");
	GLint uLoc = d_material->getShader()->getAttributeLocation("uv");

	// A location of -1 means this shader never reads the attribute, so it was optimized out.
	if (vLoc != -1)
	{
		bindAttribute(vao, vLoc, 0, d_mesh->getVertexBuffer()->getId(), 3);
	}
	if (nLoc != -1)
	{
		bindAttribute(vao, nLoc, 1, d_mesh->getNormalBuffer()->getId(), 3);
	}
	if (uLoc != -1)
	{
		bindAttribute(vao, uLoc, 2, d_mesh->getUVBuffer()->getId(), 2);
	}
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

