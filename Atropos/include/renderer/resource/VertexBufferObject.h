#pragma once

#include <glad/gl.h>

#include <cstddef>

/**
* \brief VBO of float data uploaded with GL_STATIC_DRAW.
*/
class VertexBufferObject
{
public:
	VertexBufferObject(const float* data, std::size_t size);

	VertexBufferObject(const VertexBufferObject&) = delete;
	VertexBufferObject& operator=(const VertexBufferObject&) = delete;

	~VertexBufferObject();
	
	/**
	 * \brief Binds the VBO to GL_ARRAY_BUFFER.
	 */
	void setActive() const;

	std::size_t getSize() const;

private:
	GLuint d_id;
	/**< Number of floats in the buffer. */
	std::size_t d_size;
};
