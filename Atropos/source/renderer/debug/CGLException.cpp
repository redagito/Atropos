#include "CGLException.h"

CGLException::CGLException(const char* message) throw()
	: d_message(message)
{ 
	// Empty
}

CGLException::~CGLException() throw()
{ 
	// Empty
}

const char* CGLException::what() const throw()
{
	return d_message.c_str();
}