#pragma once

#include "renderer/resource/Mesh.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/VertexArrayObject.h"


/**
* \brief Holds mesh and rendering data.
*
* A model has pointers to the model mesh and the material.
* The mesh and the material are shared resources and not managed by this class.
*/
class Model
{
public:
	/**
	* \brief Creates model from mesh and material.
	* \param mesh Pointer to the mesh data resource.
	* \param material Pointer to the material resource.
	*/
	Model(Mesh* mesh, AMaterial* material);

	/**
	* \brief Cleanup does not delete mesh or material.
	*/
	~Model();
	
	/**
	* \brief Read access to the mesh.
	*/
	const Mesh* getMesh() const;

	/**
	* \brief Read access to the material.
	*/
	const AMaterial* getMaterial() const;

	/**
	* \brief Read access to the model vertex array object.
	*/
	const VertexArrayObject* getVertexArrayObject() const;

private:
	/**
	* \brief Shared mesh resource.
	*/
	Mesh* d_mesh;

	/**
	* \brief Shared material resource.
	*/
	AMaterial* d_material;

	/**
	* \brief VAO for rendering
	*/
	VertexArrayObject d_vao;
};