#include "renderer/resource/VertexBufferObject.h"

#include "debug/Log.h"

VertexBufferObject::VertexBufferObject(const float* data, std::size_t size)
	:
	d_id(0),
	d_size(size)
{
	// Create GL buffer resource
	LOG_DEBUG("Create VertexBufferObject.");
	glGenBuffers(1, &d_id);
	
	// Bind to GL_ARRAY_BUFFER
	setActive();
	// Set data
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data, GL_STATIC_DRAW);
	return;
}

VertexBufferObject::~VertexBufferObject()
{
	LOG_DEBUG("Destroy VertexBufferObject.");
	glDeleteBuffers(1, &d_id);
}

void VertexBufferObject::setActive() const
{
	LOG_DEBUG("Set active VertexBufferObject.");
	glBindBuffer(GL_ARRAY_BUFFER, d_id);
	return;
}

std::size_t VertexBufferObject::getSize() const
{
	return d_size;
}
