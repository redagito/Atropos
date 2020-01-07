#include "CRenderbuffer.h"

CRenderbuffer::CRenderbuffer()
{
	glGenRenderbuffers(1, &d_id);
}

CRenderbuffer::~CRenderbuffer()
{
	glDeleteRenderbuffers(1, &d_id);
}

void CRenderbuffer::setActive() const
{

}
