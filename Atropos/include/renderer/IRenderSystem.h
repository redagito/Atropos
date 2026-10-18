#pragma once

class Renderable;
class Camera;

struct GLFWwindow;

#include <glm/glm.hpp>

#include <string>

/**
 * \brief Interface to the rendering system.
 *
 * Window and OpenGL device creation stay outside: they need a real context.
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	/**
	* \brief Creates a new scene, replacing any scene of the same name.
	*/
	virtual void createScene(const std::string& name) = 0;

	virtual void setActiveScene(const std::string& name) = 0;

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

	virtual void setCulling(bool cullingStatus) = 0;

	virtual bool getCulling() const = 0;

	virtual GLFWwindow* window() const = 0;

	virtual bool shouldClose() const = 0;

	/**
	* \brief Asks the window to close, which ends the main loop.
	*/
	virtual void requestClose() = 0;
};