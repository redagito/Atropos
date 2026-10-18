#pragma once

#include "resource/IResource.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>

/**
* \brief Stores RGBA image data, 4 bytes per pixel.
*/
class Image : public IResource
{
public:
	Image();

	/**
	* \brief Load image from file.
	* Throws std::runtime_error if the image cannot be loaded.
	*/
	void load(const std::string& fileName);
	
	unsigned int getWidth() const;

	unsigned int getHeight() const;

	const unsigned char* getData() const;
	
protected:
	unsigned int d_width;
	unsigned int d_height;
	std::vector<unsigned char> d_data;
};