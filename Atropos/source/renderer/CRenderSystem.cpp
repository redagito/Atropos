#include "CRenderSystem.h"

#include "CRenderable.h"
#include "io/CConfigFile.h"

#include "resource/CResourceManager.h"
#include "scene/CScene.h"
#include "scene/CCamera.h"
#include "render/CRenderer.h"

#include "debug/Log.h"
#include "debug/CProfiler.h"
#include "util/Global.h"

#include "flext/flextGL.h"

#include <glfw/glfw3.h>

#include <sstream>

void CRenderSystem::resizeCallback(GLFWwindow* window, int width, int height)
{
	if (window == Global::window)
	{
		CRenderSystem::getInstance().onResize(width, height);
	}
}

CRenderSystem::CRenderSystem()
	:
	d_manager(nullptr),
	d_renderer(nullptr),
	d_culling(false),
	d_activeScene(nullptr)
{

}

bool CRenderSystem::init(const CConfigFile& config)
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
	glfwSetFramebufferSizeCallback(Global::window, CRenderSystem::resizeCallback);

	// Create resource manager width resource path
	d_manager = std::unique_ptr<CResourceManager>(new CResourceManager(config.get("render_resourcepath", "data/")));

	// Create render device with window size and shadow mapping shader
	d_renderer = std::shared_ptr<CRenderer>(new CRenderer(width, height, d_manager->getShader("shadow")));

	// Show window
	glfwShowWindow(Global::window);
	return true;
}

CRenderSystem::~CRenderSystem()
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
void CRenderSystem::setCulling(bool status)
{
	d_culling = status;
}

bool CRenderSystem::getCulling() const
{
	return d_culling;
}

void CRenderSystem::createScene(const std::string& name)
{
	d_scenes[name] = CScene();
}

void CRenderSystem::destroyScene(const std::string& name)
{
	if (d_activeScene == &(d_scenes.at(name)))
	{
		d_activeScene = nullptr;
	}
	d_scenes.erase(name);
}

void CRenderSystem::setActiveScene(const std::string& name)
{
	assert(hasScene(name));
	d_activeScene = &(d_scenes.at(name));
}

bool CRenderSystem::hasScene(const std::string& name) const
{
	return (d_scenes.count(name) != 0);
}

// Draw call
void CRenderSystem::draw()
{
	CProfiler profiler("rendersys_draw");
	d_renderer->startFrame();
	if (d_activeScene != nullptr)
	{
		d_activeScene->draw(d_renderer, d_camera, d_culling);
	}
	d_renderer->endFrame();
}

void CRenderSystem::swapBuffer()
{
	// Buffer swap
	glfwSwapBuffers(Global::window);
}

bool CRenderSystem::loadModel(const std::string& modelName)
{
	return d_manager->loadModel(modelName);
}

// Add entity
CRenderable* CRenderSystem::add(const char* model)
{
	return add(model, "unnamed");
}

// Add named entity
CRenderable* CRenderSystem::add(const char* model, const char* name)
{
	assert(d_activeScene != nullptr);
	CRenderable* entity = new CRenderable(d_manager->getModel(model), name);
	d_activeScene->add(entity);
	return entity;
}

CCamera& CRenderSystem::getCamera()
{
	return d_camera;
}

void CRenderSystem::onResize(int width, int height)
{
	d_renderer->onResize(width, height);
	d_camera.setProjectionMatrix(d_camera.getFieldOfView(),
								(float) width / (float) height, 
								d_camera.getZNear(),
								d_camera.getZFar());
}

std::string CRenderSystem::statisticsToString() const
{
	std::stringstream ss;
	ss << "[Frame Statistics]" << std::endl;
	ss << "Draw calls: " << d_renderer->getFrameStats().drawCalls << std::endl;
	ss << "Vertex count: " << d_renderer->getFrameStats().vertexCount << std::endl;
	return ss.str();
}
