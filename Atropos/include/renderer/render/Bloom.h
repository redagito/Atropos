#pragma once

#include "renderer/render/RenderTarget.h"
#include "renderer/resource/Shader.h"

#include <glad/gl.h>

#include <memory>
#include <string>

/**
* \brief Glow around anything in the scene brighter than white.
*
* The scene arrives as a half float texture, which is what gives an additive fireball
* anything above the bright pass threshold to bloom.
*/
class Bloom
{
public:
	Bloom();

	Bloom(const Bloom&) = delete;
	Bloom& operator=(const Bloom&) = delete;

	~Bloom();

	/**
	* \brief Loads the bloom shaders. The half resolution buffers are allocated by
	 * apply, since they depend on the size of the first frame seen.
	 * Throws std::runtime_error when the sources are missing.
	*/
	void init(const std::string& resourcePath);

	/**
	* \brief Blurs the bright parts of a scene texture and adds them back, leaving
	* the result on the default framebuffer. Allocates on first use.
	*/
	void apply(GLuint sceneTexture, int sceneWidth, int sceneHeight, float strength);

private:
	/** \brief Throws away the half resolution buffers so the next apply reallocates. */
	void release();

	/** \brief Draws a full screen quad into a target, sampling source on unit 0. */
	void blit(const Shader& shader, GLuint sourceTexture, RenderTarget* target);

	Shader d_brightShader;
	Shader d_blurShader;
	Shader d_compositeShader;

	GLuint d_vao;
	GLuint d_buffer;

	std::unique_ptr<RenderTarget> d_ping;
	std::unique_ptr<RenderTarget> d_pong;
};