#pragma once

#include "renderer/flext/flextGL.h"

class CShadowBuffer
{
public:
	CShadowBuffer();
	~CShadowBuffer();

	void setActive();
	GLuint getDepthTexture() const;
	
private:
	GLuint d_shadowFramebuffer; /**< Shadow buffer id. */
	GLuint d_depthTexture; /**< Depth texture id. */
};