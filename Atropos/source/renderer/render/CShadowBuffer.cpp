#include "CShadowBuffer.h"

#include "debug/Log.h"

CShadowBuffer::CShadowBuffer()
{
	glGenFramebuffers(1, &d_shadowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);

	// Generate depth texture
	glGenTextures(1, &d_depthTexture);
	glBindTexture(GL_TEXTURE_2D, d_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// Set texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// Attach textures for depth
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d_depthTexture, 0);
	// Disable draw buffer
	glDrawBuffer(GL_NONE);
	
	// Check error
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Framebuffer not complete");
		return;
	}
	// Set framebuffer to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return;
}

CShadowBuffer::~CShadowBuffer()
{
	glDeleteFramebuffers(1, &d_shadowFramebuffer);
	glDeleteTextures(1, &d_depthTexture);
}

void CShadowBuffer::setActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, d_shadowFramebuffer);
	glViewport(0, 0, 512, 512);
}

GLuint CShadowBuffer::getDepthTexture() const
{
	return d_depthTexture;
}
