#include "renderer/RenderSystem.h"

#include "renderer/Renderable.h"
#include "io/ConfigFile.h"

#include "renderer/resource/ResourceManager.h"
#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"
#include "renderer/render/Renderer.h"

#include "debug/Profiler.h"
#include "util/Global.h"

#include <glad/glad.h>

#include <glfw/glfw3.h>

#include <stdexcept>
#include <string>

void RenderSystem::resizeCallback(GLFWwindow* window, int width, int height)
{
	if (window == Global::window)
	{
		RenderSystem::getInstance().onResize(width, height);
	}
}

RenderSystem::RenderSystem()
	:
	d_manager(nullptr),
	d_renderer(nullptr),
	d_culling(false),
	d_activeScene(nullptr)
{

}

void RenderSystem::init(const ConfigFile& config, ThreadPool& pool)
{
	const unsigned int width = config.get("render_windowwidth", 800);
	const unsigned int height = config.get("render_windowheight", 600);

	// Init GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW.");
	}
	// GL core context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 30);

	// Global window access, remove later
	Global::window = glfwCreateWindow(width, height, config.get("render_windowtitle", "Atropos").c_str(), NULL, NULL);
	if (Global::window == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the render window.");
	}
	glfwMakeContextCurrent(Global::window);

	// Vsync needs a current context.
	glfwSwapInterval(1);

	// Load OpenGL function pointers
	if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
	{
		glfwDestroyWindow(Global::window);
		Global::window = nullptr;
		glfwTerminate();
		throw std::runtime_error("Failed to load the OpenGL function pointers.");
	}

	// Resize callback
	glfwSetFramebufferSizeCallback(Global::window, RenderSystem::resizeCallback);

	// Create resource manager width resource path
	d_manager = std::unique_ptr<ResourceManager>(new ResourceManager(config.get("render_resourcepath", "data/"), pool));

	// Create render device with window size and shadow mapping shader.
	// Loading the shadow shader throws if the resource is unavailable.
	Shader* shadowShader = d_manager->getShader("shadow");
	d_renderer = std::shared_ptr<Renderer>(new Renderer(width, height, shadowShader));

	// Show window
	glfwShowWindow(Global::window);
}

RenderSystem::~RenderSystem()
{
	d_renderer = nullptr;
	d_manager = nullptr;
	d_activeScene = nullptr;
	
	// Delete rendering window
	if (Global::window != nullptr)
	{
		glfwDestroyWindow(Global::window);
		Global::window = nullptr;
	}
	glfwTerminate();
}

// Enables or disables culling in the scene
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
	d_scenes[name] = Scene();
}

void RenderSystem::setActiveScene(const std::string& name)
{
	assert(hasScene(name));
	d_activeScene = &(d_scenes.at(name));
}

bool RenderSystem::hasScene(const std::string& name) const
{
	return (d_scenes.count(name) != 0);
}

// Draw call
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
	// Buffer swap
	glfwSwapBuffers(Global::window);
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

// Add entity
Renderable* RenderSystem::add(const char* model)
{
	assert(d_activeScene != nullptr);
	Model* modelPtr = d_manager->getModel(model);
	Renderable* entity = new Renderable(modelPtr);
	d_activeScene->add(entity);
	return entity;
}

Camera& RenderSystem::getCamera()
{
	return d_camera;
}

void RenderSystem::onResize(int width, int height)
{
	// The callback can fire after the window is created but before the renderer
	// exists, so guard against it.
	if (d_renderer == nullptr)
	{
		return;
	}
	d_renderer->onResize(width, height);

	// A minimized window reports a zero height, which would produce an
	// infinite aspect ratio and an invalid projection matrix.
	if (height <= 0)
	{
		return;
	}
	d_camera.setProjectionMatrix(d_camera.getFieldOfView(),
								(float) width / (float) height, 
								d_camera.getZNear(),
								d_camera.getZFar());
}
