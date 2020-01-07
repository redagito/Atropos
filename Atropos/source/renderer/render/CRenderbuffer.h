#pragma once

#include "renderer/flext/flextGL.h"

/**
* \brief Represents a renderbuffer object to use with a framebuffer.
*/
class CRenderbuffer
{
public:
	/**
	* \brief Initializes render buffer
	*/
	CRenderbuffer();

	/**
	* \brief Cleans up gpu resources
	*/
	~CRenderbuffer();

	/**
	* \brief Binds render buffer.
	*/
	void setActive() const;

private:
	/**
	* \brief Buffer id.
	*/
	GLuint d_id;
};