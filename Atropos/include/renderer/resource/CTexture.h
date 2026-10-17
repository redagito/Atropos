#pragma once

#include "renderer/flext/flextGL.h"
#include "resource/CImage.h"

#include <string>

/**
 * \brief Represents and manages lifetime of a GL texture object.
 */
class CTexture
{
public:
	/**
	 * \brief Constructor creates texture resource.
	 */
	CTexture();

	/**
	 * \brief Frees GPU resources.
	 */
	~CTexture();

	/**
	* \brief Init from image file.
	*/
	bool init(const CImage& image);

	/**
	 * \brief Set texture active as specified texture unit.
	 */
	void setActive(unsigned int unit) const;

private:
	GLuint d_texId; /**< GL texture id. */
	GLint d_mipMapLevel; /**< Mip map level. */
	int d_width; /**< Texture width. */
	int d_height; /**< Texture height. */
};
