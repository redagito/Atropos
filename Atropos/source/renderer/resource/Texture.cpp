#include "renderer/resource/Texture.h"

#include "debug/Log.h"

Texture::Texture()
	:
	d_texId(0)
{
	// Create texture id
	glGenTextures(1, &d_texId);
}

bool Texture::init(const Image& image)
{
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, d_texId);
	
	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	
	// Load data to vram
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
	
	// Mip maps
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

// Clean up texture vram
Texture::~Texture()
{
	glDeleteTextures(1, &d_texId);
	return;
}

// Binds texture to texture unit
void Texture::setActive(unsigned int unit) const
{
	// Set active texture unit
	glActiveTexture(GL_TEXTURE0 + unit);

	// Bind
	glBindTexture(GL_TEXTURE_2D, d_texId);
	return;
}
