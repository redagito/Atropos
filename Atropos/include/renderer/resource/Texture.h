#pragma once

#include <glad/gl.h>
#include "resource/Image.h"

#include <string>

class Texture
{
public:
	Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	~Texture();

	/**
	* \brief Throws std::runtime_error if the image is empty.
	*/
	void init(const Image& image);

	void setActive(unsigned int unit) const;

private:
	GLuint d_texId;
};
