#pragma once

#include "renderer/flext/flextGL.h"

/**
* \brief Framebuffer class for offscreen rendering and renderbuffer management
*/
class CFramebuffer
{
public:
	/**
	* \brief Initializes frame buffer.
	*/
	CFramebuffer();

	/**
	* \brief Cleans up gpu resources.
	*/
	~CFramebuffer();

private:
	/**
	* \brief Framebuffer id
	*/
	GLuint d_framebufferId;
};