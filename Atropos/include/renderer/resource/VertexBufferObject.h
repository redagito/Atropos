#pragma once

#include <glad/glad.h>

#include <cstddef>

/*
* \brief Manages an OpenGL VBO in VRAM.
* Stores float triples (x, y, z) and uses static draw modifier.
*/
class VertexBufferObject
{
public:
	/**
	 * \brief Creates buffer object from float data.
	 */
	VertexBufferObject(const float* data, std::size_t size);

	VertexBufferObject(const VertexBufferObject&) = delete;
	VertexBufferObject& operator=(const VertexBufferObject&) = delete;

	/**
	 * \brief Frees all GPU resources.
	 */
	~VertexBufferObject();
	
	/**
	 * \brief Sets the VBO as active object.
	 * Binds the VBO to GL_ARRAY_BUFFER.
	 */
	void setActive() const;

	/**
	 * \brief Returns buffer size.
	 */
	std::size_t getSize() const;

private:
	GLuint d_id; /**< GL buffer object id. */
	std::size_t d_size; /**< Number of floats in the buffer. */
};
