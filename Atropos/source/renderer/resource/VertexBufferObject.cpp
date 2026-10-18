#include "renderer/resource/VertexBufferObject.h"

#include <spdlog/spdlog.h>

VertexBufferObject::VertexBufferObject(const float* data, std::size_t size)
	:
	d_id(0),
	d_size(size)
{
	SPDLOG_DEBUG("Create VertexBufferObject.");
	glGenBuffers(1, &d_id);
	
	setActive();
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data, GL_STATIC_DRAW);
	return;
}

VertexBufferObject::~VertexBufferObject()
{
	SPDLOG_DEBUG("Destroy VertexBufferObject.");
	glDeleteBuffers(1, &d_id);
}

void VertexBufferObject::setActive() const
{
	SPDLOG_DEBUG("Set active VertexBufferObject.");
	glBindBuffer(GL_ARRAY_BUFFER, d_id);
	return;
}

std::size_t VertexBufferObject::getSize() const
{
	return d_size;
}
