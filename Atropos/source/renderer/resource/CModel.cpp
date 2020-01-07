#include "CModel.h"

#include "AMaterial.h"
#include "CShader.h"
#include "CMesh.h"

#include "debug/Log.h"

/**
* Initializes the model specific VAO.
*/
CModel::CModel(CMesh* mesh, AMaterial* material)
	:
	d_mesh(mesh),
	d_material(material)
{
	// Set up the VAO for this model
	d_vao.setActive();
	
	// Get locations for vertex, normal and uv buffer
	GLuint vLoc = d_material->getShader()->getAttributeLocation("vertex");
	GLuint nLoc = d_material->getShader()->getAttributeLocation("normal");
	GLuint uLoc = d_material->getShader()->getAttributeLocation("uv");
	
	// Enable all attributes and bind the VBOs to the locations
	if (vLoc != -1)
	{
		glEnableVertexAttribArray(vLoc);
		// Vertices
		d_mesh->getVertexBuffer()->setActive();
		glVertexAttribPointer(vLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (nLoc != -1)
	{
		glEnableVertexAttribArray(nLoc);
		// Normals
		d_mesh->getNormalBuffer()->setActive();
		glVertexAttribPointer(nLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (uLoc != -1) 
	{
		glEnableVertexAttribArray(uLoc);
		// Texture coordinates, UV
		d_mesh->getUVBuffer()->setActive();
		glVertexAttribPointer(uLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	// Disables VAO
	d_vao.setInactive();
}

CModel::~CModel()
{

}

// Read access
const CVertexArrayObject* CModel::getVertexArrayObject() const
{
	return &d_vao;
}

const CMesh* CModel::getMesh() const
{
	return d_mesh;
}

const AMaterial* CModel::getMaterial() const
{
	return d_material;
}

