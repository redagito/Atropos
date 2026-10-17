#include "renderer/resource/VertexArrayObject.h"

#include <spdlog/spdlog.h>

VertexArrayObject::VertexArrayObject()
	:
	d_id(0)
{
	glGenVertexArrays(1, &d_id);
}

VertexArrayObject::~VertexArrayObject()
{
	SPDLOG_DEBUG("CVertexArray destructor call");
	glDeleteVertexArrays(1, &d_id);
}

void VertexArrayObject::setActive() const
{
	glBindVertexArray(d_id);
	return;
}

void VertexArrayObject::setInactive() const
{
	glBindVertexArray(0);
	return;
}
