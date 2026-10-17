#include "renderer/debug/CGLException.h"

CGLException::CGLException(const char* message) noexcept
	: d_message(message)
{ 
	// Empty
}

CGLException::~CGLException() noexcept
{ 
	// Empty
}

const char* CGLException::what() const throw()
{
	return d_message.c_str();
}