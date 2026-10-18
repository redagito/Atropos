#include "renderer/RenderSystem.h"

#include "renderer/GLDebug.h"

#include "renderer/Renderable.h"
#include "io/ConfigFile.h"

#include "renderer/resource/ResourceManager.h"
#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"
#include "renderer/render/Renderer.h"

#include "debug/Profiler.h"

#include <spdlog/spdlog.h>

#include <glad/gl.h>

#include <glfw/glfw3.h>

#include <stdexcept>
#include <string>

void RenderSystem::resizeCallback(GLFWwindow* window, int width, int height)
{
	RenderSystem& system = RenderSystem::getInstance();
	// GLFW can still fire this during teardown, so only our own window is acted on.
	if (system.d_window != nullptr && window == system.d_window)
	{
		system.onResize(width, height);
	}
}

RenderSystem::RenderSystem()
	:
	d_manager(nullptr),
	d_renderer(nullptr),
	d_window(nullptr),
	d_culling(false),
	d_activeScene(nullptr)
{

}

void RenderSystem::init(const ConfigFile& config, ThreadPool& pool)
{
	const unsigned int width = config.get("render_windowwidth", 800);
	const unsigned int height = config.get("render_windowheight", 600);

	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW.");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	d_window = glfwCreateWindow(width, height, config.get("render_windowtitle", "Atropos").c_str(), NULL, NULL);
	if (d_window == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create an OpenGL 4.6 core render window. "
			"The graphics driver does not provide it.");
	}
	glfwMakeContextCurrent(d_window);

	// Vsync needs a current context.
	glfwSwapInterval(1);

	// glad 2 returns the packed core profile version on success, 0 means it could not be read.
	const int gladVersion = gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress));
	if (gladVersion == 0)
	{
		glfwDestroyWindow(d_window);
		d_window = nullptr;
		glfwTerminate();
		throw std::runtime_error("Failed to load the OpenGL function pointers.");
	}

	SPDLOG_INFO("OpenGL context version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	// Nothing polls glGetError, so debug output is the only failure signal; synchronous keeps message order.
	// A GL error is always a bug, so the app stops rather than rendering wrong frames.
	d_debugSink.abortOnError = true;
	GLDebug::install(d_debugSink);

	glfwSetFramebufferSizeCallback(d_window, RenderSystem::resizeCallback);

	const std::string resourcePath = config.get("render_resourcepath", "data/");
	d_manager = std::unique_ptr<ResourceManager>(new ResourceManager(resourcePath, pool));

	// getShader throws if the "shadow" resource is missing.
	Shader* shadowShader = d_manager->getShader("shadow");
	d_renderer = std::shared_ptr<Renderer>(new Renderer(width, height, shadowShader, resourcePath));

	glfwShowWindow(d_window);

	// GLFW raises no resize event on creation; without this the camera aspect stays default.
	int framebufferWidth = 0;
	int framebufferHeight = 0;
	glfwGetFramebufferSize(d_window, &framebufferWidth, &framebufferHeight);
	if (framebufferWidth > 0 && framebufferHeight > 0)
	{
		onResize(framebufferWidth, framebufferHeight);
	}
}

RenderSystem::~RenderSystem()
{
	d_renderer = nullptr;
	d_manager = nullptr;
	d_activeScene = nullptr;

	if (d_window != nullptr)
	{
		glfwDestroyWindow(d_window);
		d_window = nullptr;
	}
	glfwTerminate();
}

GLFWwindow* RenderSystem::window() const
{
	return d_window;
}

bool RenderSystem::shouldClose() const
{
	return d_window != nullptr && glfwWindowShouldClose(d_window) != 0;
}

void RenderSystem::requestClose()
{
	if (d_window != nullptr)
	{
		glfwSetWindowShouldClose(d_window, GLFW_TRUE);
	}
}

void RenderSystem::setCulling(bool status)
{
	d_culling = status;
}

bool RenderSystem::getCulling() const
{
	return d_culling;
}

void RenderSystem::createScene(const std::string& name)
{
	// Assigning d_scenes[name] destroys the old Scene, so drop the pointer first.
	const auto existing = d_scenes.find(name);
	if (existing != d_scenes.end() && d_activeScene == &existing->second)
	{
		d_activeScene = nullptr;
	}
	d_scenes[name] = Scene();
}

void RenderSystem::setActiveScene(const std::string& name)
{
	// Check, not assert: Release compiles asserts out, so this would deref a missing scene.
	if (!hasScene(name))
	{
		throw std::runtime_error("Cannot activate a scene that does not exist: " + name);
	}
	d_activeScene = &(d_scenes.at(name));
}

bool RenderSystem::hasScene(const std::string& name) const
{
	return (d_scenes.count(name) != 0);
}

void RenderSystem::setSceneBackgroundColor(const std::string& name, const glm::vec3& color)
{
	// Check, not assert: Release compiles asserts out, so this would touch a missing scene.
	if (!hasScene(name))
	{
		throw std::runtime_error("Cannot set the background of a scene that does not exist: " + name);
	}
	d_scenes.at(name).setBackgroundColor(color);
}

void RenderSystem::draw()
{
	Profiler profiler("rendersys_draw");
	d_renderer->startFrame();
	if (d_activeScene != nullptr)
	{
		d_activeScene->draw(d_renderer, d_camera, d_culling);
	}
	d_renderer->endFrame();
}

void RenderSystem::swapBuffer()
{
	glfwSwapBuffers(d_window);
}

void RenderSystem::loadModel(const std::string& modelName)
{
	d_manager->loadModel(modelName);
}

void RenderSystem::process()
{
	d_manager->process();
}

bool RenderSystem::isIdle() const
{
	return d_manager->isIdle();
}

bool RenderSystem::hasError() const
{
	return d_manager->hasError();
}

Renderable* RenderSystem::add(const char* model)
{
	// Check, not assert: Release compiles asserts out, so this would add to no scene.
	if (d_activeScene == nullptr)
	{
		throw std::runtime_error("Cannot add a renderable without an active scene.");
	}
	Model* modelPtr = d_manager->getModel(model);
	Renderable* entity = new Renderable(modelPtr);
	d_activeScene->add(entity);
	return entity;
}

Camera& RenderSystem::getCamera()
{
	return d_camera;
}

UI& RenderSystem::ui()
{
	return d_renderer->ui();
}

void RenderSystem::onResize(int width, int height)
{
	// GLFW can fire this before the renderer exists; guard against it.
	if (d_renderer == nullptr)
	{
		return;
	}
	d_renderer->onResize(width, height);

	// A minimized window reports zero size, giving an invalid aspect ratio.
	if (width <= 0 || height <= 0)
	{
		return;
	}
	d_camera.setProjectionMatrix(d_camera.getFieldOfView(),
								(float) width / (float) height, 
								d_camera.getZNear(),
								d_camera.getZFar());
}
