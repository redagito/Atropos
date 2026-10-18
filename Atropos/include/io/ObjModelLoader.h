#pragma once

#include "collision/AABBox.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

/*
* \brief Loads and parses .obj files.
* Only fully triangulated faces are supported; missing normals and UVs become zero.
*/
class ObjModelLoader
{
public:
	ObjModelLoader();

	/** \brief Loads the model; throws std::runtime_error if the file cannot be opened or holds no geometry. */
	void load(const std::string& file);

	const std::vector<float>& getVertices() const;

	const std::vector<float>& getNormals() const;

	const std::vector<float>& getUV() const;
	
	const AABBox& getAABBox() const;

private:
	void parseLine(const std::string&, std::vector<float>& vert, std::vector<float>& norm, std::vector<float>& uv);
	void parseVertex(const std::string&, std::vector<float>&);
	void parseUV(const std::string&, std::vector<float>&);
	void parseNormal(const std::string&, std::vector<float>&);
	void parseIndex(const std::string&, const std::vector<float>&, const std::vector<float>&, const std::vector<float>&);

	void calculateAABBox();

	std::vector<float> d_vertices;
	std::vector<float> d_normals;
	std::vector<float> d_uv;
	AABBox d_box;
};