#include "CMesh.h"
#include "debug/Log.h"

#include "io/CObjModelLoader.h"

CMesh::CMesh()
	:
	d_normals(nullptr),
	d_texCoords(nullptr),
	d_vertices(nullptr)
{
	
}

CMesh::~CMesh()
{
	return;
}

bool CMesh::init(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uv, const CAABBox& box)
{
	// Load into VRAM
	d_vertices.reset(new CVertexBufferObject(vertices.data(), vertices.size()));
	d_normals.reset(new CVertexBufferObject(normals.data(), normals.size()));
	d_texCoords.reset(new CVertexBufferObject(uv.data(), uv.size()));

	// Store bounding box
	d_box = box;
	return true;
}

const CAABBox& CMesh::getAABBox() const
{
	return d_box;
}

const std::unique_ptr<CVertexBufferObject>& CMesh::getVertexBuffer() const
{
	return d_vertices;
}

const std::unique_ptr<CVertexBufferObject>& CMesh::getNormalBuffer() const
{
	return d_normals;
}

const std::unique_ptr<CVertexBufferObject>& CMesh::getUVBuffer() const
{
	return d_texCoords;
}
