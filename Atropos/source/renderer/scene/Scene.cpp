#include "renderer/scene/Scene.h"

#include "renderer/render/Renderer.h"
#include "renderer/resource/Model.h"
#include "renderer/scene/Camera.h"
#include "debug/Profiler.h"
#include "debug/Log.h"

#include <cmath>

/*
* Constructor
*/
Scene::Scene()
	:
	d_backgroundColor(1.f) // Default white
{
	return;
}

Scene::~Scene()
{
	return;
}

/*
* Adds a game object to the scene, this function takes a pointer to the heap
* The object belongs to the scene and gets deleted on scene deletion
*/
void Scene::add(Renderable* obj)
{
	d_objects.push_back(std::shared_ptr<Renderable>(obj));
}

// Checks culling state of the object
bool Scene::isCulled(const Renderable* entity, const Camera& camera) const
{
	const AABBox& box = entity->getAABBox();
	float zMin = box.getMid().z - box.getHalfWidths().z;
	float zMax = box.getMid().z + box.getHalfWidths().z;

	// z far and z near in world coordinates
	float zFarWorld = camera.getZFar() + camera.getPosition().z;
	float zNearWorld = camera.getZNear() + camera.getPosition().z;

	// Culling test
	if (zMax < zNearWorld || zMin > zFarWorld)
	{
		return true;
	}
	return false;
}

/*
* Send scene objects to renderer for drawing
*/
void Scene::draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera, bool culling)
{
	Profiler profiler("scene_draw");

	// Set view and projection
	// TODO Should the camera really belong to the scene?
	renderer->setViewMatrix(camera.getViewMatrix());
	renderer->setProjectionMatrix(camera.getProjectionMatrix());

	// Set scene background color
	renderer->setClearColor(d_backgroundColor);

	// TODO HACK for shadow mapping, remove this later
	// TODO This is actually a directional light, needs support in scene and renderer.
	renderer->setLightPosition(glm::vec3(0.f, 100.f, camera.getPosition().z + 50.f));

	for (auto iter = d_objects.begin(); iter != d_objects.end();)
	{
		// Check if the entity requested deletion
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
