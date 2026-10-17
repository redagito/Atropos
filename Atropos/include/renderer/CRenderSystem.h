#pragma once

class CRenderable; /**< Drawable entity forward declare. */
class CResourceManager; /**< Resource manager for shared rendering resources forward declare. */
class CRenderer; /**< Renderer forward declare. */
class CConfigFile; /**< Config file forward declare. */
struct GLFWwindow; /**< GLFW window forwrad declare. */

#include "scene/CScene.h"
#include "scene/CCamera.h"

#include "core/TSingletonMixin.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <thread>

/**
* \brief The rendering system manages window creation, scenes and rendering.
* The rendering system is implemented as a singleton.
*/
class CRenderSystem : public TSingletonMixin<CRenderSystem>
{
public:
	/**
	* \brief Contructor sets the resource path and screen size for resource management and rendering.
	*/
	CRenderSystem();

	/**
	* \brief Deletes all resources and entities
	*/
	~CRenderSystem();

	/**
	* \brief Initialize render system.
	*/
	bool init(const CConfigFile& config);

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
	* \brief Deletes specified scene.
	*/
	void destroyScene(const std::string& name);

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
	CRenderable* add(const char* modelName);

	/**
	* \brief Adds named entity to scene for debugging.
	*/
	CRenderable* add(const char* modelName, const char* name);

	/**
	* \brief Returns modifiable camera.
	*/
	CCamera& getCamera();

	/**
	* \brief Draw the specified scene.
	*/
	void draw();

	/**
	* \brief Waits for drawing to finish and swaps buffers.
	*/
	void swapBuffer();

	/**
	* \brief Request removal of the provided entity
	*/
	void remove(CRenderable* entity);

	/**
	* \brief Removes all renderable objects from the scene.
	*/
	void removeAll();

	/**
	* \brief Prints render stats for debug and profiling.
	*/
	std::string statisticsToString() const;

private:
	/**
	* \brief Resize callback for glfw.
	*/
	static void resizeCallback(GLFWwindow* window, int width, int height);

	/**
	* \brief Resize window callback
	*/
	void onResize(int width, int height);

	CCamera d_camera; /**< Camera object. */
	std::unordered_map<std::string, CScene> d_scenes; /**< Available scenes. */
	CScene* d_activeScene; /**< Active scene. */
	std::unique_ptr<CResourceManager> d_manager; /**< Rendering resource manager. */
	std::shared_ptr<CRenderer> d_renderer; /**< Active rendering device. */
	bool d_culling; /**< Culling mode. */
	std::thread d_renderingThread; /**< Rendering worker thread. */
};
