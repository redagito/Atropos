#include "renderer/resource/VertexArrayObject.h"

#include <spdlog/spdlog.h>

VertexArrayObject::VertexArrayObject()
	:
	d_id(0)
{
	glCreateVertexArrays(1, &d_id);
}

VertexArrayObject::~VertexArrayObject()
{
	SPDLOG_DEBUG("CVertexArray destructor call");
	glDeleteVertexArrays(1, &d_id);
}

GLuint VertexArrayObject::getId() const
{
	return d_id;
}

// A VAO still has to be bound to draw; only its setup is done without binding.
void VertexArrayObject::setActive() const
{
	glBindVertexArray(d_id);
}

void VertexArrayObject::setInactive() const
{
	glBindVertexArray(0);
}