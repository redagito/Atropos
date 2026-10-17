#pragma once

#include "renderer/IRenderSystem.h"
#include "renderer/scene/Camera.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

/**
 * \brief Render system test double that records what the game asked for.
 *
 * Mirrors the public API of IRenderSystem. There is no window or OpenGL device
 * behind it, so nothing is drawn and add() returns no drawable, which the menu
 * display handles.
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

	std::vector<std::string> d_createdScenes; /**< Names of scenes created, in order. */
	std::vector<std::string> d_requestedModels; /**< Names of models requested, in order. */
	std::vector<std::string> d_addedModels; /**< Names of models added to a scene, in order. */
	std::string d_activeScene; /**< Name of the scene last made active. */
	std::string d_lastBackgroundScene; /**< Scene whose background color was set last. */
	int d_backgroundColorCount = 0; /**< How often a background color was set. */
	bool d_culling = false; /**< Culling state. */

private:
	Camera d_camera; /**< Real camera, so states can still reset and read the view. */
};