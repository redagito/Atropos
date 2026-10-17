#pragma once

#include "renderer/flext/flextGL.h"

/*
* \brief Manages an OpenGL VBO in VRAM.
* Stores float triples (x, y, z) and uses static draw modifier.
*/
class CVertexBufferObject
{
public:
	/**
	 * \brief Creates buffer object from float data.
	 */
	CVertexBufferObject(const float* data, int size);

	/**
	 * \brief Frees all GPU resources.
	 */
	~CVertexBufferObject();
	
	/**
	 * \brief Sets the VBO as active object.
	 * Binds the VBO to GL_ARRAY_BUFFER.
	 */
	void setActive() const;

	/**
	 * \brief Access to internal buffer id.
	 * TODO Should/can this be hidden?
	 */
	GLuint getId() const;

	/**
	 * \brief Returns buffer size.
	 * TODO Why not unsigned?
	 */
	int getSize() const;

private:
	GLuint d_id; /**< GL buffer object id. */
	int d_size; /**< Number of floats in the buffer. */
};
