#pragma once

#include "renderer/resource/VertexBufferObject.h"
#include "collision/AABBox.h"

#include <memory>
#include <vector>

/**
* \brief Contains mesh data (vertices, faces, normals and uv data).
*
* Represents mesh data in the VRAM. It holds a buffer object for vertices,
* one for texture coordinates and one for nomal data. the data can be used for
* direct rendering or used in conjunction with an VAO.
*/
class Mesh
{
public:
	/**
	* \brief Sets default values.
	*/
	Mesh();
	
	/**
	* \brief Deletes graphic resources and frees memory.
	*/
	~Mesh();

	/**
	* \brief Initialize with model loader.
	*/
	bool init(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uv, const AABBox& box);

	/**
	* \brief Read access to vertex buffer
	*/
	const std::unique_ptr<VertexBufferObject>& getVertexBuffer() const;

	/**
	* \brief Read access to normal buffer
	*/
	const std::unique_ptr<VertexBufferObject>& getNormalBuffer() const;

	/**
	* \brief Read access to texture uv buffer
	*/
	const std::unique_ptr<VertexBufferObject>& getUVBuffer() const;

	/**
	* \brief Read access to the axis aligned bounding box of the model
	*/
	const AABBox& getAABBox() const;

private:
	std::unique_ptr<VertexBufferObject> d_vertices; /**< Mesh vertices. */
	std::unique_ptr<VertexBufferObject> d_normals; /**< Per vertex normals. */
	std::unique_ptr<VertexBufferObject> d_texCoords; /**< Texture coordinates. */
	AABBox d_box; /**< Bounding box. */
};
