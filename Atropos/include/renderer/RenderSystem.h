#pragma once

class Renderable; /**< Drawable entity forward declare. */
class ResourceManager; /**< Resource manager for shared rendering resources forward declare. */
class Renderer; /**< Renderer forward declare. */
class ConfigFile; /**< Config file forward declare. */
class ThreadPool; /**< Worker pool forward declare. */
struct GLFWwindow; /**< GLFW window forwrad declare. */

#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"
#include "renderer/IRenderSystem.h"

#include "core/TSingletonMixin.h"

#include <string>
#include <map>
#include <memory>

/**
* \brief The rendering system manages window creation, scenes and rendering.
* The rendering system is implemented as a singleton.
*/
class RenderSystem : public TSingletonMixin<RenderSystem>, public IRenderSystem
{
public:
	/**
	* \brief Contructor sets the resource path and screen size for resource management and rendering.
	*/
	RenderSystem();

	/**
	* \brief Deletes all resources and entities
	*/
	~RenderSystem();

	/**
	* \brief Initialize render system using the given pool for resource parsing.
	* Throws std::runtime_error if the render system cannot be initialized.
	*/
	void init(const ConfigFile& config, ThreadPool& pool);

	/**
	* \brief Enables or disables culling functionality
	*/
	void setCulling(bool cullingStatus) override;

	/**
	* \brief Returns culling state.
	*/
	bool getCulling() const override;

	/**
	* \brief Creates new scene object.
	*/
	void createScene(const std::string& name) override;

	/**
	* \brief Sets the active scene by name.
	*/
	void setActiveScene(const std::string& name) override;

	/**
	* \brief True if a scene with the specified name exists and false otherwise.
	*/
	bool hasScene(const std::string& name) const override;

	/**
	* \brief Sets the color a scene is cleared to.
	*
	* Throws std::runtime_error if the scene does not exist.
	*/
	void setSceneBackgroundColor(const std::string& name, const glm::vec3& color) override;

	/**
	* \brief Loads model into resource cache.
	* Loads model resource. Throws std::runtime_error on failure.
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

	/**
	* \brief True if any requested resource failed to load.
	*/
	bool hasError() const override;

	/**
	* \brief Add a new drawable entity to the scene.
	* The requested model is loaded if necessary and a new instance of the model
	* is created as an entity in the scene and a proxy of that entity
	* is returned.
	*/
	Renderable* add(const char* modelName) override;

	/**
	* \brief Returns modifiable camera.
	*/
	Camera& getCamera() override;

	/**
	* \brief Draw the specified scene.
	*/
	void draw() override;

	/**
	* \brief Waits for drawing to finish and swaps buffers.
	*/
	void swapBuffer() override;

	/**
	* \brief Returns the render window.
	*/
	GLFWwindow* window() const override;

	/**
	* \brief True once the window has been asked to close.
	*/
	bool shouldClose() const override;

	/**
	* \brief Asks the window to close, which ends the main loop.
	*/
	void requestClose() override;

private:
	/**
	* \brief Resize callback for glfw.
	*/
	static void resizeCallback(GLFWwindow* window, int width, int height);

	/**
	* \brief Resize window callback
	*/
	void onResize(int width, int height);

	Camera d_camera; /**< Camera object. */
	// A node based map keeps the address of an existing scene valid when another
	// scene is added, which the active scene pointer relies on. An unordered_map
	// can rehash and invalidate it.
	std::map<std::string, Scene> d_scenes; /**< Available scenes. */
	Scene* d_activeScene; /**< Active scene. */
	std::unique_ptr<ResourceManager> d_manager; /**< Rendering resource manager. */
	std::shared_ptr<Renderer> d_renderer; /**< Active rendering device. */
	GLFWwindow* d_window; /**< The render window this system owns. */
	bool d_culling; /**< Culling mode. */
};
