#pragma once

#include <glad/gl.h>

/**
* \brief VAO storing and restoring VBO binding state.
*/
class VertexArrayObject
{
public:
	VertexArrayObject();

	VertexArrayObject(const VertexArrayObject&) = delete;
	VertexArrayObject& operator=(const VertexArrayObject&) = delete;

	~VertexArrayObject();
	
	GLuint getId() const;

	void setActive() const;

	void setInactive() const;

private:
	GLuint d_id;
};
