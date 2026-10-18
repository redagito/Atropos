#pragma once

#include <glad/gl.h>

/**
* \brief KHR_debug setup. Every message is logged by severity, and GL errors
* either abort the process or are counted so a test can assert on them.
*/
class GLDebug
{
public:
	/** \brief Where the callback reports to and what it does about errors. */
	struct Sink
	{
		/** \brief GL errors seen since install; a test asserts this stays zero. */
		int errors = 0;

		/** \brief Call std::abort on the first GL error instead of only counting. */
		bool abortOnError = false;
	};

	/**
	* \brief Enables synchronous debug output and routes messages into sink, which
	* must outlive every GL call made afterwards. Notification severity is dropped
	* because it is driver chatter that hides the real messages.
	*/
	static void install(Sink& sink);

	static void GLAD_API_PTR callback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};