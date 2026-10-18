#pragma once

#include "renderer/resource/VertexBufferObject.h"
#include "renderer/resource/MeshData.h"
#include "collision/AABBox.h"

#include <memory>
#include <vector>

/**
* \brief Contains mesh data in the VRAM.
*
* Holds one buffer object each for vertices, normals and uv, bound through a VAO.
*/
class Mesh
{
public:
	Mesh();
	
	~Mesh();

	/**
	* \brief Throws std::runtime_error if the vertex data is empty.
	*/
	void init(const MeshData& data);

	const std::unique_ptr<VertexBufferObject>& getVertexBuffer() const;

	const std::unique_ptr<VertexBufferObject>& getNormalBuffer() const;

	const std::unique_ptr<VertexBufferObject>& getUVBuffer() const;

	const AABBox& getAABBox() const;

private:
	std::unique_ptr<VertexBufferObject> d_vertices;
	std::unique_ptr<VertexBufferObject> d_normals;
	std::unique_ptr<VertexBufferObject> d_texCoords;
	AABBox d_box;
};
