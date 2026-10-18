#include "renderer/render/RenderTarget.h"

#include <stdexcept>

RenderTarget::RenderTarget(int width, int height, bool withDepth)
	:
	d_width(width),
	d_height(height),
	d_framebuffer(0),
	d_colourTexture(0),
	d_depthTexture(0)
{
	if (width <= 0 || height <= 0)
	{
		throw std::runtime_error("Render target needs a positive size.");
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &d_colourTexture);
	glTextureStorage2D(d_colourTexture, 1, GL_RGBA16F, width, height);

	glTextureParameteri(d_colourTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(d_colourTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(d_colourTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(d_colourTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glCreateFramebuffers(1, &d_framebuffer);
	glNamedFramebufferTexture(d_framebuffer, GL_COLOR_ATTACHMENT0, d_colourTexture, 0);

	if (withDepth)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &d_depthTexture);
		glTextureStorage2D(d_depthTexture, 1, GL_DEPTH_COMPONENT24, width, height);
		glNamedFramebufferTexture(d_framebuffer, GL_DEPTH_ATTACHMENT, d_depthTexture, 0);
	}

	// Checking named state still needs the target bound, but nothing has to be drawn.
	glBindFramebuffer(GL_FRAMEBUFFER, d_framebuffer);
	if (glCheckNamedFramebufferStatus(d_framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		throw std::runtime_error("Render target is not complete. The GL driver may not support float colour buffers.");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
	if (d_framebuffer != 0)
	{
		glDeleteFramebuffers(1, &d_framebuffer);
	}
	if (d_colourTexture != 0)
	{
		glDeleteTextures(1, &d_colourTexture);
	}
	if (d_depthTexture != 0)
	{
		glDeleteTextures(1, &d_depthTexture);
	}
}

void RenderTarget::setActive() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, d_framebuffer);
	glViewport(0, 0, d_width, d_height);
}

GLuint RenderTarget::getColourTexture() const
{
	return d_colourTexture;
}

GLuint RenderTarget::getFramebufferId() const
{
	return d_framebuffer;
}

int RenderTarget::getWidth() const
{
	return d_width;
}

int RenderTarget::getHeight() const
{
	return d_height;
}