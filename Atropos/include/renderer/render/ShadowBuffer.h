#pragma once

#include "renderer/flext/flextGL.h"

class ShadowBuffer
{
public:
	ShadowBuffer();
	~ShadowBuffer();

	void setActive();
	GLuint getDepthTexture() const;
	
private:
	GLuint d_shadowFramebuffer; /**< Shadow buffer id. */
	GLuint d_depthTexture; /**< Depth texture id. */
};