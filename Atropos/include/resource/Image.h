#pragma once

#include "resource/IResource.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>

/**
* \brief Stores RGBA image data.
* Currently supports loading from tga files.
*/
class Image : public IResource
{
public:
	/**
	* \brief Default constructor.
	*/
	Image();

	/**
	* \brief Load image from file.
	* Throws std::runtime_error if the image cannot be loaded.
	*/
	void load(const std::string& fileName);
	
	/**
	* \brief Returns image width.
	*/
	unsigned int getWidth() const;

	/**
	* \brief Returns image height.
	*/
	unsigned int getHeight() const;

	/**
	* \brief Returns image data.
	*/
	const unsigned char* getData() const;
	
protected:
	unsigned int d_width; /**< Image width. */
	unsigned int d_height; /**< Image height. */
	std::vector<unsigned char> d_data; /**< Image data. */
};