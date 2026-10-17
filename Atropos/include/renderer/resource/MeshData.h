#pragma once

#include "collision/AABBox.h"

#include <vector>

/**
* \brief CPU side mesh data.
*
* Produced by a loader and uploaded to the GPU by Mesh::init. Holding the data
* separately keeps parsing (which may run on a worker thread) apart from the
* OpenGL upload (which must run on the thread with the current context).
*/
struct MeshData
{
	std::vector<float> vertices; /**< Interleaved vertex positions. */
	std::vector<float> normals; /**< Per vertex normals. */
	std::vector<float> uv; /**< Texture coordinates. */
	AABBox box; /**< Bounding box of the mesh. */
};
