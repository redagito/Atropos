#pragma once

// Forward declares
class Renderable;
class Camera;

#include <glm/glm.hpp>

#include <string>

/**
 * \brief Interface to the rendering system.
 *
 * Covers the scene and resource handling the game asks for. Creating the window
 * and the OpenGL device itself stays outside the interface, because those need
 * a real context.
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	/**
	* \brief Creates a new scene, replacing any scene of the same name.
	*/
	virtual void createScene(const std::string& name) = 0;

	/**
	* \brief Sets the active scene by name.
	*/
	virtual void setActiveScene(const std::string& name) = 0;

	/**
	* \brief True if a scene with the specified name exists.
	*/
	virtual bool hasScene(const std::string& name) const = 0;

	/**
	* \brief Sets the color a scene is cleared to.
	*/
	virtual void setSceneBackgroundColor(const std::string& name, const glm::vec3& color) = 0;

	/**
	* \brief Requests a model resource.
	*/
	virtual void loadModel(const std::string& modelName) = 0;

	/**
	* \brief Commits finished resources. Must run on the main thread.
	*/
	virtual void process() = 0;

	/**
	* \brief True when all requested resources are ready or failed.
	*/
	virtual bool isIdle() const = 0;

	/**
	* \brief True if any requested resource failed to load.
	*/
	virtual bool hasError() const = 0;

	/**
	* \brief Adds a drawable to the active scene and returns it.
	*/
	virtual Renderable* add(const char* modelName) = 0;

	/**
	* \brief Returns modifiable camera.
	*/
	virtual Camera& getCamera() = 0;

	/**
	* \brief Draws the active scene.
	*/
	virtual void draw() = 0;

	/**
	* \brief Waits for drawing to finish and swaps buffers.
	*/
	virtual void swapBuffer() = 0;

	/**
	* \brief Enables or disables culling.
	*/
	virtual void setCulling(bool cullingStatus) = 0;

	/**
	* \brief Returns the culling state.
	*/
	virtual bool getCulling() const = 0;
};