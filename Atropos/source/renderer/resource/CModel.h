#pragma once

#include "CMesh.h"
#include "AMaterial.h"
#include "CVertexArrayObject.h"


/**
* \brief Holds mesh and rendering data.
*
* A model has pointers to the model mesh and the material.
* The mesh and the material are shared resources and not managed by this class.
*/
class CModel
{
public:
	/**
	* \brief Creates model from mesh and material.
	* \param mesh Pointer to the mesh data resource.
	* \param material Pointer to the material resource.
	*/
	CModel(CMesh* mesh, AMaterial* material);

	/**
	* \brief Cleanup does not delete mesh or material.
	*/
	~CModel();
	
	/**
	* \brief Read access to the mesh.
	*/
	const CMesh* getMesh() const;

	/**
	* \brief Read access to the material.
	*/
	const AMaterial* getMaterial() const;

	/**
	* \brief Read access to the model vertex array object.
	*/
	const CVertexArrayObject* getVertexArrayObject() const;

private:
	/**
	* \brief Shared mesh resource.
	*/
	CMesh* d_mesh;

	/**
	* \brief Shared material resource.
	*/
	AMaterial* d_material;

	/**
	* \brief VAO for rendering
	*/
	CVertexArrayObject d_vao;
};