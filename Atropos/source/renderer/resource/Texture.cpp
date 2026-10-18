#include "renderer/resource/Texture.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

Texture::Texture()
	:
	d_texId(0)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &d_texId);
}

void Texture::init(const Image& image)
{
	if (image.getWidth() == 0 || image.getHeight() == 0)
	{
		throw std::runtime_error("Cannot initialize a texture from an empty image.");
	}

	const GLsizei width = static_cast<GLsizei>(image.getWidth());
	const GLsizei height = static_cast<GLsizei>(image.getHeight());
	// Immutable storage has to reserve the whole mip chain, and GL_LINEAR_MIPMAP_LINEAR needs all of it.
	const GLsizei levels = static_cast<GLsizei>(std::log2(static_cast<double>(std::max(width, height)))) + 1;

	glTextureParameteri(d_texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(d_texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(d_texId, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTextureParameteri(d_texId, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	glTextureStorage2D(d_texId, levels, GL_RGBA8, width, height);
	glTextureSubImage2D(d_texId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

	glGenerateTextureMipmap(d_texId);
}

Texture::~Texture()
{
	glDeleteTextures(1, &d_texId);
	return;
}

void Texture::setActive(unsigned int unit) const
{
	glBindTextureUnit(unit, d_texId);
}
