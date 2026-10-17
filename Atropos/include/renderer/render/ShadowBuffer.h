#pragma once

#include <glad/glad.h>

class ShadowBuffer
{
public:
	ShadowBuffer();
	~ShadowBuffer();

	ShadowBuffer(const ShadowBuffer&) = delete;
	ShadowBuffer& operator=(const ShadowBuffer&) = delete;

	void setActive();
	GLuint getDepthTexture() const;
	
private:
	GLuint d_shadowFramebuffer; /**< Shadow buffer id. */
	GLuint d_depthTexture; /**< Depth texture id. */
};