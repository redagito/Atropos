#include "renderer/render/ShadowBuffer.h"

#include <stdexcept>

ShadowBuffer::ShadowBuffer()
	:
	d_shadowFramebuffer(0),
	d_depthTexture(0)
{
	glGenFramebuffers(1, &d_shadowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);

	glGenTextures(1, &d_depthTexture);
	glBindTexture(GL_TEXTURE_2D, d_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d_depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &d_shadowFramebuffer);
		glDeleteTextures(1, &d_depthTexture);
		d_shadowFramebuffer = 0;
		d_depthTexture = 0;
		throw std::runtime_error("Shadow framebuffer is not complete.");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowBuffer::~ShadowBuffer()
{
	glDeleteFramebuffers(1, &d_shadowFramebuffer);
	glDeleteTextures(1, &d_depthTexture);
}

void ShadowBuffer::setActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);
	glViewport(0, 0, 512, 512);
}

GLuint ShadowBuffer::getDepthTexture() const
{
	return d_depthTexture;
}
