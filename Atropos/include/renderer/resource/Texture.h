#pragma once

#include <glad/glad.h>
#include "resource/Image.h"

#include <string>

/**
 * \brief Represents and manages lifetime of a GL texture object.
 */
class Texture
{
public:
	/**
	 * \brief Constructor creates texture resource.
	 */
	Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	/**
	 * \brief Frees GPU resources.
	 */
	~Texture();

	/**
	* \brief Init from image file.
	*/
	bool init(const Image& image);

	/**
	 * \brief Set texture active as specified texture unit.
	 */
	void setActive(unsigned int unit) const;

private:
	GLuint d_texId; /**< GL texture id. */
};
