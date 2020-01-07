#include "CVertexBufferObject.h"

#include "debug/Log.h"

CVertexBufferObject::CVertexBufferObject(const float* data, int size)
	:
	d_size(size)
{
	// Create GL buffer resource
	LOG_DEBUG("Create CVertexBufferObject.");
	glGenBuffers(1, &d_id);
	
	// Bind to GL_ARRAY_BUFFER
	setActive();
	// Set data
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data, GL_STATIC_DRAW);
	return;
}

CVertexBufferObject::~CVertexBufferObject()
{
	LOG_DEBUG("Destroy CVertexBufferObject.");
	glDeleteBuffers(1, &d_id);
}

void CVertexBufferObject::setActive() const
{
	LOG_DEBUG("Set active CVertexBufferObject.");
	glBindBuffer(GL_ARRAY_BUFFER, d_id);
	return;
}

GLuint CVertexBufferObject::getId() const
{
	return d_id;
}

int CVertexBufferObject::getSize() const
{
	return d_size;
}
