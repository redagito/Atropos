#include "renderer/render/Sky.h"

#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include <array>
#include <cstddef>

Sky::Sky()
	:
	d_buffer(0),
	d_vao(0),
	d_zenith(0.25f, 0.50f, 0.80f),
	d_horizon(0.70f, 0.82f, 0.90f)
{
}

Sky::~Sky()
{
	// Guarded so an uninitialised sky can be destroyed without a context.
	if (d_vao != 0)
	{
		glDeleteVertexArrays(1, &d_vao);
	}
	if (d_buffer != 0)
	{
		glDeleteBuffers(1, &d_buffer);
	}
}

void Sky::init(const std::string& resourcePath)
{
	FileLoader vertexLoader(resourcePath + "shader/src/sky.vs");
	vertexLoader.load();
	FileLoader fragmentLoader(resourcePath + "shader/src/sky.fs");
	fragmentLoader.load();

	ShaderSources sources;
	sources.vertex = vertexLoader.getContent();
	sources.fragment = fragmentLoader.getContent();
	d_shader.init(sources);

	glCreateBuffers(1, &d_buffer);
	glCreateVertexArrays(1, &d_vao);
	glVertexArrayVertexBuffer(d_vao, 0, d_buffer, 0, sizeof(Vertex));

	glVertexArrayAttribFormat(d_vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(d_vao, 0, 0);
	glEnableVertexArrayAttrib(d_vao, 0);

	glVertexArrayAttribFormat(d_vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	glVertexArrayAttribBinding(d_vao, 1, 0);
	glEnableVertexArrayAttrib(d_vao, 1);

	rebuild();
}

void Sky::setColors(const glm::vec3& zenith, const glm::vec3& horizon)
{
	d_zenith = zenith;
	d_horizon = horizon;
	rebuild();
}

void Sky::rebuild()
{
	if (d_buffer == 0)
	{
		return;
	}

	// Two triangles covering clip space, zenith at the top row.
	const std::array<Vertex, 6> vertices = {{
		{glm::vec2(-1.f, 1.f), glm::vec4(d_zenith, 1.f)},
		{glm::vec2(1.f, 1.f), glm::vec4(d_zenith, 1.f)},
		{glm::vec2(-1.f, -1.f), glm::vec4(d_horizon, 1.f)},

		{glm::vec2(-1.f, -1.f), glm::vec4(d_horizon, 1.f)},
		{glm::vec2(1.f, 1.f), glm::vec4(d_zenith, 1.f)},
		{glm::vec2(1.f, -1.f), glm::vec4(d_horizon, 1.f)},
	}};

	glNamedBufferData(d_buffer, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
}

void Sky::draw()
{
	d_shader.setActive();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);

	glBindVertexArray(d_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Restored for the geometry pass that follows.
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}