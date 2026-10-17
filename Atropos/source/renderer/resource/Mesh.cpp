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

void Mesh::init(const MeshData& data)
{
	if (data.vertices.empty())
	{
		throw std::runtime_error("Cannot initialize a mesh without vertex data.");
	}

	// Load into VRAM
	d_vertices.reset(new VertexBufferObject(data.vertices.data(), data.vertices.size()));
	d_normals.reset(new VertexBufferObject(data.normals.data(), data.normals.size()));
	d_texCoords.reset(new VertexBufferObject(data.uv.data(), data.uv.size()));

	// Store bounding box
	d_box = data.box;
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
