#pragma once

#include "collision/AABBox.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

/*
* \brief Loads and parses .obj files.
*
* Provides access to verticex, normal and uv data.
* Obj files must be fully triangulated, hold only one model and contain normals and UVs.
*/
class ObjModelLoader
{
public:
	/**
	* \brief Default constructor.
	*/
	ObjModelLoader();

	/**
	* \brief Loads model from stream.
	*/
	bool load(std::string& file);

	/**
	* \brief Returns vertex data.
	*/
	const std::vector<float>& getVertices() const;

	/**
	* \brief Returns normal data.
	*/
	const std::vector<float>& getNormals() const;

	/**
	* \brief Returns UV data.
	*/
	const std::vector<float>& getUV() const;
	
	/**
	* \brief Returns bounding volume.
	*/
	const AABBox& getAABBox() const;

private:
	// Parse single line
	void parseLine(const std::string&, std::vector<float>& vert, std::vector<float>& norm, std::vector<float>& uv);
	// Parse data
	void parseVertex(const std::string&, std::vector<float>&);
	void parseUV(const std::string&, std::vector<float>&);
	void parseNormal(const std::string&, std::vector<float>&);
	void parseIndex(const std::string&, const std::vector<float>&, const std::vector<float>&, const std::vector<float>&);

	/**
	* \brief Calculates bounding box from vertex data.
	*/
	void calculateAABBox();

	std::vector<float> d_vertices; /**< Vertices. */
	std::vector<float> d_normals; /**< Normal data. */
	std::vector<float> d_uv; /**< Texture coordinate data. */
	AABBox d_box; /**< Axis aligned bounding box for the loaded mesh. */
};