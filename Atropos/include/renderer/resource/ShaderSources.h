#pragma once

#include <string>

/**
* \brief Vertex and fragment source text; only reading is thread safe,
* compiling and linking need the current OpenGL context thread.
*/
struct ShaderSources
{
	std::string vertex;
	std::string fragment;
};
