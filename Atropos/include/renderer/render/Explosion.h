#pragma once

#include "renderer/resource/Shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

/**
* \brief Additive fireball bursts, drawn after the scene geometry and before the HUD.
*
* Particles are owned here instead of the game world, so an enemy can be erased the
* frame it dies without cutting its explosion short.
*/
class Explosion
{
public:
	Explosion();

	Explosion(const Explosion&) = delete;
	Explosion& operator=(const Explosion&) = delete;

	~Explosion();

	/**
	* \brief Loads the particle shader and allocates the pool.
	 * Throws std::runtime_error when the sources are missing.
	*/
	void init(const std::string& resourcePath);

	/** \brief Starts a burst of particles at a world position. */
	void addExplosion(const glm::vec3& position);

	void update(float dtime);

	/**
* \brief Draws every live particle; the caller binds the framebuffer and viewport first.
* Unfogged on purpose: an explosion is gameplay feedback, and fading it into the
* horizon hides the kill that the sound already announced.
*/
	void draw(const glm::mat4& view, const glm::mat4& proj);

	/** \brief Number of particles still within their lifetime. */
	int liveParticleCount() const;

private:
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		float age;
		float lifetime;
		float seed;
	};

	/** \brief Six of these make up the quad of a single particle. */
	struct Vertex
	{
		glm::vec2 corner;
		glm::vec3 position;
		float age;
		float lifetime;
		float size;
		float seed;
	};

	/** \brief Drops every particle past its lifetime and packs the rest to the front. */
	void compact();

	Shader d_shader;
	GLuint d_buffer;
	GLuint d_vao;

	std::vector<Particle> d_particles;
	std::vector<Vertex> d_vertices;

	unsigned int d_seed;
};