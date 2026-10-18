#include "renderer/resource/VertexBufferObject.h"

#include <spdlog/spdlog.h>

VertexBufferObject::VertexBufferObject(const float* data, std::size_t size)
	:
	d_id(0),
	d_size(size)
{
	SPDLOG_DEBUG("Create VertexBufferObject.");
	glCreateBuffers(1, &d_id);
	glNamedBufferStorage(d_id, size*sizeof(float), data, 0);
	return;
}

VertexBufferObject::~VertexBufferObject()
{
	SPDLOG_DEBUG("Destroy VertexBufferObject.");
	glDeleteBuffers(1, &d_id);
}

GLuint VertexBufferObject::getId() const
{
	return d_id;
}

std::size_t VertexBufferObject::getSize() const
{
	return d_size;
}