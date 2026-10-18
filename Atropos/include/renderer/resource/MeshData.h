#pragma once

#include "collision/AABBox.h"

#include <vector>

/**
* \brief CPU side mesh data. The upload via Mesh::init must run on the thread
* with the current OpenGL context, parsing may run on a worker thread.
*/
struct MeshData
{
	std::vector<float> vertices;
	/**< Per vertex normals. */
	std::vector<float> normals;
	std::vector<float> uv;
	AABBox box;
};
