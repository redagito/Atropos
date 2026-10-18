#pragma once

class Renderer;
class Camera;

#include "renderer/Renderable.h"

#include <memory>
#include <list>

class Scene
{
public:
	Scene();

	~Scene();

	/** 
	* \brief Takes ownership of node. The scene deletes it, so the caller must not.
	*/
	void add(Renderable* node);

	/** 
	* \brief Draws all objects, culling against the camera when \p culling is set.
	*/
	void draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera, bool culling = true);

	void setBackgroundColor(const glm::vec3& color);

	const glm::vec3& getBackgroundColor() const;

	/**
	* \brief Requests the zenith to horizon gradient behind the scene. Off by default,
	* so a scene never inherits one another scene switched on.
	*/
	void setSky(bool enabled, const glm::vec3& zenith, const glm::vec3& horizon);

	bool isSkyEnabled() const;

private:
	bool isCulled(const Renderable* entity, const Camera& camera) const;

	std::list<std::shared_ptr<Renderable>> d_objects;
	glm::vec3 d_backgroundColor;
	bool d_skyEnabled;
	glm::vec3 d_zenith;
	glm::vec3 d_horizon;
};
