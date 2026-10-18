#pragma once

#include "renderer/resource/Mesh.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/VertexArrayObject.h"


/**
* \brief Holds mesh and rendering data.
*
* The mesh and the material are shared resources and not managed by this class.
*/
class Model
{
public:
	/**
	* \brief Does not take ownership of mesh or material.
	*/
	Model(Mesh* mesh, AMaterial* material);

	~Model();
	
	const Mesh* getMesh() const;

	const AMaterial* getMaterial() const;

	const VertexArrayObject* getVertexArrayObject() const;

private:
	Mesh* d_mesh;

	AMaterial* d_material;

	VertexArrayObject d_vao;
};