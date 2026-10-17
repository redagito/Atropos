#include "renderer/RenderSystem.h"

#include "renderer/Renderable.h"
#include "io/ConfigFile.h"

#include "renderer/resource/ResourceManager.h"
#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"
#include "renderer/render/Renderer.h"

#include "debug/Log.h"
#include "debug/Profiler.h"
#include "util/Global.h"

#include "renderer/flext/flextGL.h"

#include <glfw/glfw3.h>

#include <sstream>

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

bool RenderSystem::init(const ConfigFile& config)
{
	unsigned int width = config.get("render_windowwidth", 800);
	unsigned int height = config.get("render_windowheight", 600);

	// Init GLFW
	if (!glfwInit())
	{
		return false;
	}
	// GL core context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, FLEXT_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, FLEXT_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 30);
	glfwSwapInterval(30);

	// Global window access, remove later
	Global::window = glfwCreateWindow(width, height, config.get("render_windowtitle", "Atropos").c_str(), NULL, NULL);
	if (Global::window == nullptr)
	{
		return false;
	}
	glfwMakeContextCurrent(Global::window);

	// Load GL extensions
	if (!flextInit())
	{
		return false;
	}

	// Resize callback
	glfwSetFramebufferSizeCallback(Global::window, RenderSystem::resizeCallback);

	// Create resource manager width resource path
	d_manager = std::unique_ptr<ResourceManager>(new ResourceManager(config.get("render_resourcepath", "data/")));

	// Create render device with window size and shadow mapping shader
	d_renderer = std::shared_ptr<Renderer>(new Renderer(width, height, d_manager->getShader("shadow")));

	// Show window
	glfwShowWindow(Global::window);
	return true;
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

bool RenderSystem::loadModel(const std::string& modelName)
{
	return d_manager->loadModel(modelName);
}

// Add entity
Renderable* RenderSystem::add(const char* model)
{
	assert(d_activeScene != nullptr);
	Model* modelPtr = d_manager->getModel(model);
	if (modelPtr == nullptr)
	{
		LOG_ERROR("Failed to add entity: model %s could not be loaded.", model);
		return nullptr;
	}
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
	d_renderer->onResize(width, height);
	d_camera.setProjectionMatrix(d_camera.getFieldOfView(),
								(float) width / (float) height, 
								d_camera.getZNear(),
								d_camera.getZFar());
}
