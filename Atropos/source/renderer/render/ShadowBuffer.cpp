#include "renderer/render/ShadowBuffer.h"

#include <stdexcept>

ShadowBuffer::ShadowBuffer()
	:
	d_shadowFramebuffer(0),
	d_depthTexture(0)
{
	glCreateFramebuffers(1, &d_shadowFramebuffer);

	glCreateTextures(GL_TEXTURE_2D, 1, &d_depthTexture);
	glTextureStorage2D(d_depthTexture, 1, GL_DEPTH_COMPONENT16, 512, 512);

	glTextureParameteri(d_depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(d_depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(d_depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(d_depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(d_depthTexture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTextureParameteri(d_depthTexture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glNamedFramebufferTexture(d_shadowFramebuffer, GL_DEPTH_ATTACHMENT, d_depthTexture, 0);

	// Draw buffer state belongs to the bound target and has no named form, so bind just to set it.
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckNamedFramebufferStatus(d_shadowFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteFramebuffers(1, &d_shadowFramebuffer);
		glDeleteTextures(1, &d_depthTexture);
		d_shadowFramebuffer = 0;
		d_depthTexture = 0;
		throw std::runtime_error("Shadow framebuffer is not complete.");
	}
}

ShadowBuffer::~ShadowBuffer()
{
	glDeleteFramebuffers(1, &d_shadowFramebuffer);
	glDeleteTextures(1, &d_depthTexture);
}

// The framebuffer still has to be bound to draw into it; only its setup is named.
void ShadowBuffer::setActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);
	glViewport(0, 0, 512, 512);
}

GLuint ShadowBuffer::getDepthTexture() const
{
	return d_depthTexture;
}

GLuint ShadowBuffer::getFramebufferId() const
{
	return d_shadowFramebuffer;
}