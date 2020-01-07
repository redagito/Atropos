#include "CVertexArrayObject.h"

#include "debug/Log.h"

CVertexArrayObject::CVertexArrayObject()
{
	glGenVertexArrays(1, &d_id);
}

CVertexArrayObject::~CVertexArrayObject()
{
	LOG_DEBUG("CVertexArray destructor call");
	glDeleteVertexArrays(1, &d_id);
}

void CVertexArrayObject::setActive() const
{
	glBindVertexArray(d_id);
	return;
}

void CVertexArrayObject::setInactive() const
{
	glBindVertexArray(0);
	return;
}

GLuint CVertexArrayObject::getId() const
{
	return d_id;
}
