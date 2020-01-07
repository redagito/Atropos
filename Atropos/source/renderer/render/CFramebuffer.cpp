#include "CFramebuffer.h"

#include "renderer/resource/CTexture.h"
#include "CRenderbuffer.h"

CFramebuffer::CFramebuffer()
{
	glGenFramebuffers(1, &d_framebufferId);
}

CFramebuffer::~CFramebuffer()
{
	glDeleteFramebuffers(1, &d_framebufferId);
}