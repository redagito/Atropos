#pragma once

#include "renderer/resource/Shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

/**
* \brief Zenith to horizon gradient drawn after the clear and before the scene,
* so the background reads as sky rather than a flat colour.
*/
class Sky
{
public:
	Sky();

	Sky(const Sky&) = delete;
	Sky& operator=(const Sky&) = delete;

	~Sky();

	/**
	* \brief Loads the gradient shader.
	 * Throws std::runtime_error when the sources are missing.
	*/
	void init(const std::string& resourcePath);

	void setColors(const glm::vec3& zenith, const glm::vec3& horizon);

	/**
	* \brief Off by default, so scenes that set a flat background colour keep it.
	*/
	void setEnabled(bool enabled);

	/** \brief True when the gradient is drawn instead of the clear colour. */
	bool isEnabled() const;

	/** \brief Draws the gradient; the caller clears and sets the viewport first. */
	void draw();

private:
	void rebuild();

	struct Vertex
	{
		glm::vec2 position;
		glm::vec4 color;
	};

	Shader d_shader;
	GLuint d_buffer;
	GLuint d_vao;
	bool d_enabled;
	glm::vec3 d_zenith;
	glm::vec3 d_horizon;
};