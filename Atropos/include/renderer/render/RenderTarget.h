#pragma once

#include <glad/gl.h>

/**
* \brief Colour framebuffer the scene is rendered into before post processing.
* Half float, so an additive fireball can exceed the range an 8 bit target would clip.
*/
class RenderTarget
{
public:
	/**
	* \brief Allocates the colour texture and, when asked, a depth texture.
	 * Throws std::runtime_error when the framebuffer is not complete.
	*/
	RenderTarget(int width, int height, bool withDepth);

	~RenderTarget();

	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;

	/** \brief Binds the framebuffer and sets the viewport to cover it. */
	void setActive() const;

	GLuint getColourTexture() const;

	GLuint getFramebufferId() const;

	int getWidth() const;

	int getHeight() const;

private:
	int d_width;
	int d_height;
	GLuint d_framebuffer;
	GLuint d_colourTexture;
	GLuint d_depthTexture;
};