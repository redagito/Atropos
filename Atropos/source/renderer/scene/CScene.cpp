#include "CScene.h"

#include "renderer/render/CRenderer.h"
#include "renderer/resource/CModel.h"
#include "CCamera.h"
#include "debug/CProfiler.h"
#include "debug/Log.h"

#include <cmath>

/*
* Constructor
*/
CScene::CScene()
	:
	d_backgroundColor(1.f) // Default white
{
	return;
}

CScene::~CScene()
{
	return;
}

/*
* Adds a game object to the scene, this function takes a pointer to the heap
* The object belongs to the scene and gets deleted on scene deletion
*/
void CScene::add(CRenderable* obj)
{
	d_objects.push_back(std::shared_ptr<CRenderable>(obj));
}

void CScene::clear()
{
	d_objects.clear();
}

void CScene::setBackgroundColor(const glm::vec3& color)
{
	d_backgroundColor = color;
}

// Checks culling state of the object
bool CScene::isCulled(const CRenderable* entity, const CCamera& camera) const
{
	const CAABBox& box = entity->getAABBox();
	float zMin = box.getMid().z - box.getHalfWidths().z;
	float zMax = box.getMid().z + box.getHalfWidths().z;
	float xMin = box.getMid().x - box.getHalfWidths().x;
	float xMax = box.getMid().x + box.getHalfWidths().x;

	// z far and z near in world coordinates
	float zFarWorld = camera.getZFar() + camera.getPosition().z;
	float zNearWorld = camera.getZNear() + camera.getPosition().z;
	float xFarWorld = camera.getZFar() + camera.getPosition().x;
	float xNearWorld = camera.getZFar() + camera.getPosition().x;

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
void CScene::draw(const std::shared_ptr<CRenderer>& renderer, const CCamera& camera, bool culling)
{
	CProfiler profiler("scene_draw");

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
			if (!isCulled((*iter).get(), camera)) // Attempt object culling
			{
				(*iter)->setCullingState(false);
				// Object might be set to invisible
				if ((*iter)->getVisibility())
				{
					// Draw call
					(*iter)->draw(renderer, camera);
				}
			}
			else
			{
				// Object was culled, set state accordingly
				(*iter)->setCullingState(true);
			}
			++iter;
		}
	}
	return;
}
