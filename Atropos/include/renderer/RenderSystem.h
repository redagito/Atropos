#pragma once

class Renderable;
class ResourceManager;
class Renderer;
class ConfigFile;
class ThreadPool;
struct GLFWwindow;

#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"
#include "renderer/IRenderSystem.h"
#include "renderer/ui/UI.h"

#include "core/TSingletonMixin.h"

#include <glad/gl.h>

#include "renderer/GLDebug.h"

#include <string>
#include <map>
#include <memory>

/**
 * \brief The rendering system manages window creation, scenes and rendering.
 * Implements IRenderSystem as a singleton.
 */
class RenderSystem : public TSingletonMixin<RenderSystem>, public IRenderSystem
{
public:
	RenderSystem();

	~RenderSystem();

	/**
	* \brief Initialize render system using the given pool for resource parsing.
	* Throws std::runtime_error if the render system cannot be initialized.
	*/
	void init(const ConfigFile& config, ThreadPool& pool);

	void setCulling(bool cullingStatus) override;

	bool getCulling() const override;

	/**
	* \brief Creates a scene, replacing and destroying any existing one of that name.
	*/
	void createScene(const std::string& name) override;

	void setActiveScene(const std::string& name) override;

	bool hasScene(const std::string& name) const override;

	/**
	* \brief Sets the color a scene is cleared to.
	* Throws std::runtime_error if the scene does not exist.
	*/
	void setSceneBackgroundColor(const std::string& name, const glm::vec3& color) override;

	/**
	* \brief Requests a model resource.
	* Throws std::runtime_error on failure.
	*/
	void loadModel(const std::string& modelName) override;

	/**
	* \brief Commits finished resources. Must run on the main thread.
	*/
	void process() override;

	/**
	* \brief True when all requested resources are loaded or failed.
	*/
	bool isIdle() const override;

	bool hasError() const override;

	/**
	* \brief Adds a drawable to the active scene, loading its model if needed.
	* Returns a proxy for the created entity.
	*/
	Renderable* add(const char* modelName) override;

	/**
	* \brief Returns modifiable camera.
	*/
	Camera& getCamera() override;

	UI& ui() override;

	void draw() override;

	/**
	* \brief Waits for drawing to finish and swaps buffers.
	*/
	void swapBuffer() override;

	GLFWwindow* window() const override;

	bool shouldClose() const override;

	/**
	* \brief Asks the window to close, which ends the main loop.
	*/
	void requestClose() override;

private:
	static void resizeCallback(GLFWwindow* window, int width, int height);

	void onResize(int width, int height);

	GLDebug::Sink d_debugSink;

	Camera d_camera;
	// Must stay a node-based map: rehashing would invalidate d_activeScene.
	std::map<std::string, Scene> d_scenes; /**< Available scenes. */
	Scene* d_activeScene; /**< Active scene. */
	std::unique_ptr<ResourceManager> d_manager; /**< Rendering resource manager. */
	std::shared_ptr<Renderer> d_renderer; /**< Active rendering device. */
	GLFWwindow* d_window; /**< The render window this system owns. */
	bool d_culling;
};
