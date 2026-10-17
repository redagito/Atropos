#pragma once

// Forward declares
class Renderer;
class Camera;

#include "renderer/Renderable.h"

#include <memory>
#include <list>

/**
* \brief Stores all entities in the current scene for rendering.
*/
class Scene
{
public:
	/**
	* \brief Initializes scene.
	*/
	Scene();

	/**
	* \brief Deletes all objects in the scene.
	*
	* The destructor calls removes all objects from the scene and frees their
	* memory.
	*/
	~Scene();

	/** 
	* \brief Adds a node to the scene.
	*
	* Adds a scene node, created on the heap, to the scene. 
	* The scene manages the deletion of added nodes. 
	* Calling delete on a node added to the scene causes undefined behaviour. 
	* \param node Pointer to the node to be added
	*/
	void add(Renderable* node);

	/** 
	* \brief Renders the scene
	*
	* The draw call traverses the scene and culls the objects against the camera then 
	* calls draw on each visible entity with the render device as argument for drawing.
	* \param renderer The render device used for drawing.
	* \param camera The active camera
	* \param culling Culling flag to enable/disable culling
	*/
	void draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera, bool culling = true);

private:
	/**
	* \brief Checks culling state for scene object.
	* Always returns false if d_culling is set to false
	*/
	bool isCulled(const Renderable* entity, const Camera& camera) const;

	std::list<std::shared_ptr<Renderable>> d_objects; /**< Current objects in scene. */
	glm::vec3 d_backgroundColor; /**< Scene background color. */
};
