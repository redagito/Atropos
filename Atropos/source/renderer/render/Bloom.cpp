#include "renderer/render/Bloom.h"

#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include <array>
#include <cstddef>
#include <utility>

namespace
{
	/** \brief Bloom runs at half resolution; the blur hides the missing detail. */
	const int BLOOM_DIVISOR = 2;

	/**
	* \brief Separable passes per axis, each widening the halo further. Below four
	* the halo stays narrower than the saturated core it surrounds, so all of it
	* lands on pixels that are already white and cannot get brighter.
	*/
	const int BLUR_ITERATIONS = 6;

	/** \brief Scene luminance above this blooms. The sky sits below one. */
	const float BRIGHT_THRESHOLD = 1.f;
}

Bloom::Bloom()
	:
	d_vao(0),
	d_buffer(0)
{
}

Bloom::~Bloom()
{
	release();

	// Guarded so an uninitialised bloom can be destroyed without a context.
	if (d_vao != 0)
	{
		glDeleteVertexArrays(1, &d_vao);
	}
	if (d_buffer != 0)
	{
		glDeleteBuffers(1, &d_buffer);
	}
}

void Bloom::init(const std::string& resourcePath)
{
	FileLoader vertexLoader(resourcePath + "shader/src/bloom.vs");
	vertexLoader.load();
	const std::string vertex = vertexLoader.getContent();

	FileLoader brightLoader(resourcePath + "shader/src/bloomBright.fs");
	brightLoader.load();
	FileLoader blurLoader(resourcePath + "shader/src/bloomBlur.fs");
	blurLoader.load();
	FileLoader compositeLoader(resourcePath + "shader/src/bloomComposite.fs");
	compositeLoader.load();

	ShaderSources brightSources;
	brightSources.vertex = vertex;
	brightSources.fragment = brightLoader.getContent();
	d_brightShader.init(brightSources);

	ShaderSources blurSources;
	blurSources.vertex = vertex;
	blurSources.fragment = blurLoader.getContent();
	d_blurShader.init(blurSources);

	ShaderSources compositeSources;
	compositeSources.vertex = vertex;
	compositeSources.fragment = compositeLoader.getContent();
	d_compositeShader.init(compositeSources);

	// Two triangles covering clip space, which every stage here samples through.
	const std::array<glm::vec2, 6> corners = {{
		{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
		{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f},
	}};

	glCreateBuffers(1, &d_buffer);
	glNamedBufferData(d_buffer, static_cast<GLsizeiptr>(corners.size() * sizeof(glm::vec2)),
		corners.data(), GL_STATIC_DRAW);
	glCreateVertexArrays(1, &d_vao);
	glVertexArrayVertexBuffer(d_vao, 0, d_buffer, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(d_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(d_vao, 0, 0);
	glEnableVertexArrayAttrib(d_vao, 0);
}

void Bloom::apply(GLuint sceneTexture, int sceneWidth, int sceneHeight, float strength)
{
	if (d_vao == 0)
	{
		return;
	}

	const int halfWidth = sceneWidth / BLOOM_DIVISOR;
	const int halfHeight = sceneHeight / BLOOM_DIVISOR;

	// A minimised window reports no size, and nothing could be sampled anyway.
	if (halfWidth <= 0 || halfHeight <= 0)
	{
		return;
	}

	if (!d_ping || d_ping->getWidth() != halfWidth || d_ping->getHeight() != halfHeight)
	{
		release();
		d_ping = std::make_unique<RenderTarget>(halfWidth, halfHeight, false);
		d_pong = std::make_unique<RenderTarget>(halfWidth, halfHeight, false);
	}

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	d_brightShader.setShaderValueFloat("threshold", BRIGHT_THRESHOLD);
	blit(d_brightShader, sceneTexture, d_ping.get());

	for (int i = 0; i < BLUR_ITERATIONS; ++i)
	{
		d_blurShader.setShaderValueVec2("direction", glm::vec2(1.f, 0.f));
		blit(d_blurShader, d_ping->getColourTexture(), d_pong.get());

		d_blurShader.setShaderValueVec2("direction", glm::vec2(0.f, 1.f));
		blit(d_blurShader, d_pong->getColourTexture(), d_ping.get());
	}

	d_compositeShader.setActive();
	d_compositeShader.setShaderValueFloat("strength", strength);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, sceneWidth, sceneHeight);
	glBindTextureUnit(0, sceneTexture);
	d_compositeShader.setShaderTexture("source", 0);
	glBindTextureUnit(1, d_ping->getColourTexture());
	d_compositeShader.setShaderTexture("blur", 1);
	glBindVertexArray(d_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Restored for the passes the renderer runs on the next frame.
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Bloom::blit(const Shader& shader, GLuint sourceTexture, RenderTarget* target)
{
	shader.setActive();
	target->setActive();

	glBindTextureUnit(0, sourceTexture);
	shader.setShaderTexture("source", 0);

	glBindVertexArray(d_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Bloom::release()
{
	d_ping.reset();
	d_pong.reset();
}