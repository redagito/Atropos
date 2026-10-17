#pragma once

#include "IResource.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>

/**
* \brief Stores RGBA image data.
* Currently supports loading from and saving to png files.
*/
class CImage : public IResource
{
public:
	/**
	* \brief Default constructor.
	*/
	CImage();

	/**
	* \brief Load image from file.
	*/
	bool load(const std::string& fileName);
	
	/**
	* \brief Initializes image to specified size and sets color.
	*/
	void init(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	/**
	* \brief Initialize image from data.
	*/
	void init(unsigned int width, unsigned int height, unsigned char* data);
	
	/**
	* \brief Sets pixel to specified value.
	*/
	void set(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	/**
	* \brief Save image to file.
	*/
	bool save(const std::string& fileName) const;
	
	/**
	* \brief Get red color value at position.
	*/
	unsigned char getRed(unsigned int width, unsigned int height) const;
	
	/**
	* \brief Get green color value at position.
	*/
	unsigned char getGreen(unsigned int width, unsigned int height) const;
	
	/**
	* \brief Get blue color value at position.
	*/
	unsigned char getBlue(unsigned int width, unsigned int height) const;
	
	/**
	* \brief Get alpha value at position.
	*/
	unsigned char getAlpha(unsigned int width, unsigned int height) const;

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