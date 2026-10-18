#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>

#include <glfw/glfw3.h>

#include "renderer/GLDebug.h"

#include <catch2/catch_test_macros.hpp>

/**
* \brief Hidden 4.6 core context for tests that need real OpenGL state.
*/
class GLTestContext
{
public:
	GLTestContext()
	{
		REQUIRE(glfwInit() == GLFW_TRUE);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		d_window = glfwCreateWindow(64, 64, "test context", nullptr, nullptr);
		REQUIRE(d_window != nullptr);
		glfwMakeContextCurrent(d_window);
		REQUIRE(glfwGetCurrentContext() != nullptr);

		// glad 2 returns the packed core profile version, not 0 for success.
		d_version = gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress));
		REQUIRE(d_version != 0);

		// A GL error here is a real bug, so fail the test rather than only logging it.
		d_sink.abortOnError = false;
		GLDebug::install(d_sink);
	}

	~GLTestContext()
	{
		glfwDestroyWindow(d_window);
		glfwTerminate();
	}

	GLTestContext(const GLTestContext&) = delete;
	GLTestContext& operator=(const GLTestContext&) = delete;

	const GLDebug::Sink& sink() const
	{
		return d_sink;
	}

	int version() const
	{
		return d_version;
	}

private:
	GLFWwindow* d_window = nullptr;
	int d_version = 0;
	GLDebug::Sink d_sink;
};