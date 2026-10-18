#include "renderer/render/Explosion.h"

#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include <glm/ext.hpp>

#include <array>
#include <cmath>
#include <cstddef>

namespace
{
	const int PARTICLES_PER_BURST = 48;
	const int MAX_PARTICLES = 1024;
	const int VERTS_PER_PARTICLE = 6;

	const float LIFETIME = 0.6f;
	const float MIN_SPEED = 5.f;
	const float MAX_SPEED = 11.f;
	const float DRAG = 2.5f;
	const float MAX_SIZE = 2.f;

	const std::array<glm::vec2, VERTS_PER_PARTICLE> kCorners = {{
		{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
		{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f},
	}};

	unsigned int xorshift32(unsigned int& state)
	{
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;
		return state;
	}

	/** \brief Xorshift32 in [0,1); unsigned division is enough at this precision. */
	float unitFloat(unsigned int& state)
	{
		return static_cast<float>(xorshift32(state) & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
	}
}

Explosion::Explosion()
	:
	d_buffer(0),
	d_vao(0),
	d_seed(0x9E3779B9u)
{
}

Explosion::~Explosion()
{
	// Guarded so an uninitialised explosion can be destroyed without a context.
	if (d_vao != 0)
	{
		glDeleteVertexArrays(1, &d_vao);
	}
	if (d_buffer != 0)
	{
		glDeleteBuffers(1, &d_buffer);
	}
}

void Explosion::init(const std::string& resourcePath)
{
	FileLoader vertexLoader(resourcePath + "shader/src/explosion.vs");
	vertexLoader.load();
	FileLoader fragmentLoader(resourcePath + "shader/src/explosion.fs");
	fragmentLoader.load();

	ShaderSources sources;
	sources.vertex = vertexLoader.getContent();
	sources.fragment = fragmentLoader.getContent();
	d_shader.init(sources);

	d_particles.reserve(MAX_PARTICLES);
	d_vertices.resize(MAX_PARTICLES * VERTS_PER_PARTICLE);

	glCreateBuffers(1, &d_buffer);
	glNamedBufferData(d_buffer, static_cast<GLsizeiptr>(d_vertices.size() * sizeof(Vertex)), nullptr, GL_DYNAMIC_DRAW);
	glCreateVertexArrays(1, &d_vao);
	glVertexArrayVertexBuffer(d_vao, 0, d_buffer, 0, sizeof(Vertex));

	glVertexArrayAttribFormat(d_vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, corner));
	glVertexArrayAttribBinding(d_vao, 0, 0);
	glEnableVertexArrayAttrib(d_vao, 0);

	glVertexArrayAttribFormat(d_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(d_vao, 1, 0);
	glEnableVertexArrayAttrib(d_vao, 1);

	glVertexArrayAttribFormat(d_vao, 2, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, age));
	glVertexArrayAttribBinding(d_vao, 2, 0);
	glEnableVertexArrayAttrib(d_vao, 2);

	glVertexArrayAttribFormat(d_vao, 3, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, lifetime));
	glVertexArrayAttribBinding(d_vao, 3, 0);
	glEnableVertexArrayAttrib(d_vao, 3);

	glVertexArrayAttribFormat(d_vao, 4, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, size));
	glVertexArrayAttribBinding(d_vao, 4, 0);
	glEnableVertexArrayAttrib(d_vao, 4);

	glVertexArrayAttribFormat(d_vao, 5, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, seed));
	glVertexArrayAttribBinding(d_vao, 5, 0);
	glEnableVertexArrayAttrib(d_vao, 5);
}

void Explosion::addExplosion(const glm::vec3& position)
{
	// No pool means nothing can be drawn, so drop the burst rather than fail later.
	if (d_buffer == 0)
	{
		return;
	}

	const int room = MAX_PARTICLES - static_cast<int>(d_particles.size());
	const int count = room < PARTICLES_PER_BURST ? room : PARTICLES_PER_BURST;

	for (int i = 0; i < count; ++i)
	{
		// Uniform over the sphere: picking the height and the angle separately is
		// what keeps the burst from bunching up at the poles.
		const float z = 2.f * unitFloat(d_seed) - 1.f;
		const float angle = 2.f * glm::pi<float>() * unitFloat(d_seed);
		const float ring = std::sqrt(1.f - z * z);

		const float speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * unitFloat(d_seed);
		// Spread the lifetimes, or the whole burst vanishes on a single frame.
		const float lifetime = LIFETIME * (0.6f + 0.8f * unitFloat(d_seed));

		Particle particle;
		particle.position = position;
		particle.velocity = glm::vec3(ring * std::cos(angle), ring * std::sin(angle), z) * speed;
		particle.age = 0.f;
		particle.lifetime = lifetime;
		particle.seed = unitFloat(d_seed);

		d_particles.push_back(particle);
	}
}

void Explosion::update(float dtime)
{
	const float drag = std::exp(-DRAG * dtime);

	for (Particle& particle : d_particles)
	{
		particle.age += dtime;
		particle.position += particle.velocity * dtime;
		particle.velocity *= drag;
	}

	compact();
}

void Explosion::compact()
{
	std::size_t live = 0;
	for (std::size_t i = 0; i < d_particles.size(); ++i)
	{
		if (d_particles[i].age < d_particles[i].lifetime)
		{
			d_particles[live] = d_particles[i];
			++live;
		}
	}
	d_particles.resize(live);
}

int Explosion::liveParticleCount() const
{
	return static_cast<int>(d_particles.size());
}

void Explosion::draw(const glm::mat4& view, const glm::mat4& proj)
{
	if (d_particles.empty())
	{
		return;
	}

	std::size_t out = 0;
	for (const Particle& particle : d_particles)
	{
		// Grows as the square root of age, so the fireball opens fast then settles.
		const float size = MAX_SIZE * std::sqrt(particle.age / particle.lifetime);

		for (const glm::vec2& corner : kCorners)
		{
			Vertex& vertex = d_vertices[out++];
			vertex.corner = corner;
			vertex.position = particle.position;
			vertex.age = particle.age;
			vertex.lifetime = particle.lifetime;
			vertex.size = size;
			vertex.seed = particle.seed;
		}
	}

	glNamedBufferSubData(d_buffer, 0, static_cast<GLsizeiptr>(out * sizeof(Vertex)), d_vertices.data());

	d_shader.setActive();
	d_shader.setShaderValueMat4("view", view);
	d_shader.setShaderValueMat4("proj", proj);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(d_vao);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(out));
	glBindVertexArray(0);

	// Restored for the HUD flush that follows.
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}