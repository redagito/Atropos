#pragma once

#include "CVertexBufferObject.h"
#include "collision/CAABBox.h"

#include <memory>
#include <vector>

/**
* \brief Contains mesh data (vertices, faces, normals and uv data).
*
* Represents mesh data in the VRAM. It holds a buffer object for vertices,
* one for texture coordinates and one for nomal data. the data can be used for
* direct rendering or used in conjunction with an VAO.
*/
class CMesh
{
public:
	/**
	* \brief Sets default values.
	*/
	CMesh();
	
	/**
	* \brief Deletes graphic resources and frees memory.
	*/
	~CMesh();

	/**
	* \brief Initialize with model loader.
	*/
	bool init(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uv, const CAABBox& box);

	/**
	* \brief Read access to vertex buffer
	*/
	const std::unique_ptr<CVertexBufferObject>& getVertexBuffer() const;

	/**
	* \brief Read access to normal buffer
	*/
	const std::unique_ptr<CVertexBufferObject>& getNormalBuffer() const;

	/**
	* \brief Read access to texture uv buffer
	*/
	const std::unique_ptr<CVertexBufferObject>& getUVBuffer() const;

	/**
	* \brief Read access to the axis aligned bounding box of the model
	*/
	const CAABBox& getAABBox() const;

private:
	std::unique_ptr<CVertexBufferObject> d_vertices; /**< Mesh vertices. */
	std::unique_ptr<CVertexBufferObject> d_normals; /**< Per vertex normals. */
	std::unique_ptr<CVertexBufferObject> d_texCoords; /**< Texture coordinates. */
	CAABBox d_box; /**< Bounding box. */
};
