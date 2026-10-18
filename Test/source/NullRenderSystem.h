#pragma once

#include "renderer/IRenderSystem.h"
#include "renderer/scene/Camera.h"
#include "renderer/ui/UI.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

/**
 * \brief Render system test double: no window or GL device behind it, so nothing
 * is drawn and add() returns nullptr, which the menu display handles.
 */
class NullRenderSystem : public IRenderSystem
{
public:
	void createScene(const std::string& name) override
	{
		d_createdScenes.push_back(name);
	}

	void setActiveScene(const std::string& name) override
	{
		d_activeScene = name;
	}

	bool hasScene(const std::string&) const override
	{
		return false;
	}

	void setSceneBackgroundColor(const std::string& name, const glm::vec3&) override
	{
		++d_backgroundColorCount;
		d_lastBackgroundScene = name;
	}

	void loadModel(const std::string& modelName) override
	{
		d_requestedModels.push_back(modelName);
	}

	void process() override
	{
	}

	bool isIdle() const override
	{
		return true;
	}

	bool hasError() const override
	{
		return false;
	}

	Renderable* add(const char* modelName) override
	{
		d_addedModels.push_back(modelName);
		return nullptr;
	}

	Camera& getCamera() override
	{
		return d_camera;
	}

	UI& ui() override
	{
		return d_ui;
	}

	void draw() override
	{
	}

	void swapBuffer() override
	{
	}

	void setCulling(bool cullingStatus) override
	{
		d_culling = cullingStatus;
	}

	bool getCulling() const override
	{
		return d_culling;
	}

	GLFWwindow* window() const override
	{
		return nullptr;
	}

	bool shouldClose() const override
	{
		return d_shouldClose;
	}

	void requestClose() override
	{
		d_shouldClose = true;
	}

	std::vector<std::string> d_createdScenes; /**< Names of scenes created, in order. */
	std::vector<std::string> d_requestedModels; /**< Names of models requested, in order. */
	std::vector<std::string> d_addedModels; /**< Names of models added to a scene, in order. */
	std::string d_activeScene; /**< Name of the scene last made active. */
	std::string d_lastBackgroundScene; /**< Scene whose background color was set last. */
	int d_backgroundColorCount = 0; /**< How often a background color was set. */
	bool d_culling = false; /**< Culling state. */
	bool d_shouldClose = false; /**< Whether a close was requested. */

private:
	Camera d_camera; /**< Real camera, so states can still reset and read the view. */
	UI d_ui; /**< Uninitialised; drawing into it is a no-op without a context. */
};