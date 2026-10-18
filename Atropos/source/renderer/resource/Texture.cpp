#include "renderer/resource/Texture.h"

#include <stdexcept>

Texture::Texture()
	:
	d_texId(0)
{
	glGenTextures(1, &d_texId);
}

void Texture::init(const Image& image)
{
	if (image.getWidth() == 0 || image.getHeight() == 0)
	{
		throw std::runtime_error("Cannot initialize a texture from an empty image.");
	}

	glBindTexture(GL_TEXTURE_2D, d_texId);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &d_texId);
	return;
}

void Texture::setActive(unsigned int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);

	glBindTexture(GL_TEXTURE_2D, d_texId);
	return;
}
