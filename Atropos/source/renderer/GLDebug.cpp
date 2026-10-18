#include "renderer/GLDebug.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <string>

void GLDebug::install(Sink& sink)
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&GLDebug::callback, &sink);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
}

void GLAD_API_PTR GLDebug::callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei, const GLchar* message, const void* userParam)
{
	const std::string text(message != nullptr ? message : "");
	const Sink* sink = static_cast<const Sink*>(userParam);

	if (type == GL_DEBUG_TYPE_ERROR)
	{
		if (sink != nullptr)
		{
			const_cast<Sink*>(sink)->errors++;
		}
		SPDLOG_ERROR("OpenGL error source {} id {}: {}", source, id, text);

		if (sink != nullptr && sink->abortOnError)
		{
			SPDLOG_CRITICAL("OpenGL reported an error; aborting.");
			std::abort();
		}
		return;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
	case GL_DEBUG_SEVERITY_MEDIUM:
		SPDLOG_WARN("OpenGL warning type {} id {}: {}", type, id, text);
		break;
	default:
		SPDLOG_DEBUG("OpenGL message type {} id {}: {}", type, id, text);
		break;
	}
}