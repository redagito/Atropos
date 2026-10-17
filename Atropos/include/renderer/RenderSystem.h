#pragma once

class Renderable; /**< Drawable entity forward declare. */
class ResourceManager; /**< Resource manager for shared rendering resources forward declare. */
class Renderer; /**< Renderer forward declare. */
class ConfigFile; /**< Config file forward declare. */
struct GLFWwindow; /**< GLFW window forwrad declare. */

#include "renderer/scene/Scene.h"
#include "renderer/scene/Camera.h"

#include "core/TSingletonMixin.h"

#include <string>
#include <memory>
#include <unordered_map>

/**
* \brief The rendering system manages window creation, scenes and rendering.
* The rendering system is implemented as a singleton.
*/
class RenderSystem : public TSingletonMixin<RenderSystem>
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
	* \brief Initialize render system.
	*/
	bool init(const ConfigFile& config);

	/**
	* \brief Enables or disables culling functionality
	*/
	void setCulling(bool cullingStatus);

	/**
	* \brief Returns culling state.
	*/
	bool getCulling() const;

	/**
	* \brief Creates new scene object.
	*/
	void createScene(const std::string& name);

	/**
	* \brief Sets the active scene by name.
	*/
	void setActiveScene(const std::string& name);

	/**
	* \brief True if a scene with the specified name exists and false otherwise.
	*/
	bool hasScene(const std::string& name) const;

	/**
	* \brief Loads model into resource cache.
	* Loads model resource.
	*/
	bool loadModel(const std::string& modelName);

	/**
	* \brief Add a new drawable entity to the scene.
	* The requested model is loaded if necessary and a new instance of the model
	* is created as an entity in the scene and a proxy of that entity
	* is returned.
	*/
	Renderable* add(const char* modelName);

	/**
	* \brief Returns modifiable camera.
	*/
	Camera& getCamera();

	/**
	* \brief Draw the specified scene.
	*/
	void draw();

	/**
	* \brief Waits for drawing to finish and swaps buffers.
	*/
	void swapBuffer();

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
	std::unordered_map<std::string, Scene> d_scenes; /**< Available scenes. */
	Scene* d_activeScene; /**< Active scene. */
	std::unique_ptr<ResourceManager> d_manager; /**< Rendering resource manager. */
	std::shared_ptr<Renderer> d_renderer; /**< Active rendering device. */
	bool d_culling; /**< Culling mode. */
};
