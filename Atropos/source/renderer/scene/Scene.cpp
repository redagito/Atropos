#include "renderer/scene/Scene.h"

#include "renderer/render/Renderer.h"
#include "renderer/resource/Model.h"
#include "renderer/scene/Camera.h"
#include "debug/Profiler.h"
#include <spdlog/spdlog.h>

#include <cmath>

Scene::Scene()
	:
	d_backgroundColor(1.f), // Default white
	d_skyEnabled(false),
	d_zenith(0.25f, 0.50f, 0.80f),
	d_horizon(0.70f, 0.82f, 0.90f)
{
	return;
}

Scene::~Scene()
{
	return;
}

void Scene::add(Renderable* obj)
{
	d_objects.push_back(std::shared_ptr<Renderable>(obj));
}

bool Scene::isCulled(const Renderable* entity, const Camera& camera) const
{
	const AABBox& box = entity->getAABBox();
	float zMin = box.getMid().z - box.getHalfWidths().z;
	float zMax = box.getMid().z + box.getHalfWidths().z;

	// z far and z near in world coordinates
	float zFarWorld = camera.getZFar() + camera.getPosition().z;
	float zNearWorld = camera.getZNear() + camera.getPosition().z;

	if (zMax < zNearWorld || zMin > zFarWorld)
	{
		return true;
	}
	return false;
}

void Scene::setBackgroundColor(const glm::vec3& color)
{
	d_backgroundColor = color;
}

const glm::vec3& Scene::getBackgroundColor() const
{
	return d_backgroundColor;
}

void Scene::setSky(bool enabled, const glm::vec3& zenith, const glm::vec3& horizon)
{
	d_skyEnabled = enabled;
	d_zenith = zenith;
	d_horizon = horizon;
}

bool Scene::isSkyEnabled() const
{
	return d_skyEnabled;
}

void Scene::draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera, bool culling)
{
	Profiler profiler("scene_draw");

	// TODO Should the camera really belong to the scene?
	renderer->setViewMatrix(camera.getViewMatrix());
	renderer->setProjectionMatrix(camera.getProjectionMatrix());

	renderer->setClearColor(d_backgroundColor);

	// The sky and the fog colour it implies are scene state, so each scene has to
	// push its own or one scene's settings would survive into the next.
	renderer->setSky(d_skyEnabled, d_zenith, d_horizon);

	// TODO HACK for shadow mapping, replace this with directional light support in Scene and Renderer.
	renderer->setLightPosition(glm::vec3(0.f, 100.f, camera.getPosition().z + 50.f));

	for (auto iter = d_objects.begin(); iter != d_objects.end();)
	{
		if ((*iter)->deleteRequested())
		{
			iter = d_objects.erase(iter);
		}
		else
		{
			const bool culled = culling && isCulled((*iter).get(), camera);
			if (!culled)
			{
				(*iter)->draw(renderer, camera);
			}
			++iter;
		}
	}
	return;
}
