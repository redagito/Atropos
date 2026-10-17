#include <exception>
#include <string>

/*
* Exception class for OpenGL errors
*/
class CGLException : public std::exception
{	
public:
	CGLException(const char* message) throw();
	virtual ~CGLException() throw();
	virtual const char* what() const throw();
private:
	const std::string d_message;
};