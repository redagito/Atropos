#include "CTexture.h"

#include "debug/Log.h"

#include "io/TgaLoader.h"

#include <stdexcept>
#include <fstream>
#include <vector>

CTexture::CTexture()
	:
	d_mipMapLevel(0),
	d_texId(0),
	d_height(0),
	d_width(0)
{
	// Create texture id
	glGenTextures(1, &d_texId);
}

bool CTexture::init(const CImage& image)
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
CTexture::~CTexture()
{
	glDeleteTextures(1, &d_texId);
	return;
}

// Binds texture to texture unit
void CTexture::setActive(unsigned int unit) const
{
	// Set active texture unit
	glActiveTexture(GL_TEXTURE0 + unit);

	// Bind
	glBindTexture(GL_TEXTURE_2D, d_texId);
	return;
}

GLuint CTexture::getId() const
{
	return d_texId;
}

GLint CTexture::getMipMapLevel() const
{
	return d_mipMapLevel;
}
