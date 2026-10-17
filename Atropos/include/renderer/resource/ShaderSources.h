#pragma once

#include <string>

/**
* \brief CPU side shader source.
*
* Holds the vertex and fragment shader source text. Reading the source can be
* done on a worker thread, compiling and linking must run on the thread with
* the current OpenGL context.
*/
struct ShaderSources
{
	std::string vertex; /**< Vertex shader source text. */
	std::string fragment; /**< Fragment shader source text. */
};
