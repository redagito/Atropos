#include "renderer/resource/Mesh.h"

#include <stdexcept>
#include <vector>

Mesh::Mesh()
	:
	d_vertices(nullptr),
	d_normals(nullptr),
	d_texCoords(nullptr)
{
	
}

Mesh::~Mesh()
{
	return;
}

void Mesh::init(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uv, const AABBox& box)
{
	if (vertices.empty())
	{
		throw std::runtime_error("Cannot initialize a mesh without vertex data.");
	}

	// Load into VRAM
	d_vertices.reset(new VertexBufferObject(vertices.data(), vertices.size()));
	d_normals.reset(new VertexBufferObject(normals.data(), normals.size()));
	d_texCoords.reset(new VertexBufferObject(uv.data(), uv.size()));

	// Store bounding box
	d_box = box;
}

const AABBox& Mesh::getAABBox() const
{
	return d_box;
}

const std::unique_ptr<VertexBufferObject>& Mesh::getVertexBuffer() const
{
	return d_vertices;
}

const std::unique_ptr<VertexBufferObject>& Mesh::getNormalBuffer() const
{
	return d_normals;
}

const std::unique_ptr<VertexBufferObject>& Mesh::getUVBuffer() const
{
	return d_texCoords;
}
