#pragma once

// Forward declares
class CRenderer;
class CCamera;

#include "renderer/CRenderable.h"

#include <memory>
#include <list>

/**
* \brief Stores all entities in the current scene for culling rendering. 
*
* The scene stores the scene entities in a spatial hierarchy for culling and
* efficient rendering.
*/
class CScene
{
public:
	/**
	* \brief Initializes scene.
	*/
	CScene();

	/**
	* \brief Deletes all objects in the scene.
	*
	* The destructor calls removes all objects from the scene and frees their
	* memory.
	*/
	~CScene();

	/** 
	* \brief Adds a node to the scene.
	*
	* Adds a scene node, created on the heap, to the scene. 
	* The scene manages the deletion of added nodes. 
	* Calling delete on a node added to the scene causes undefined behaviour. 
	* \param node Pointer to the node to be added
	*/
	void add(CRenderable* node);

	/**
	* \brief Removes all objects from the scene.
	*/
	void clear();

	/**
	 * \brief Scene background color.
	 */
	void setBackgroundColor(const glm::vec3& color);
	
	/** 
	* \brief Renders the scene
	*
	* The draw call traverses the scene and culls the objects against the camera then 
	* calls draw on each visible entity with the render device as argument for drawing.
	* \param renderer The render device used for drawing.
	* \param camera The active camera
	* \param culling Culling flag to enable/disable culling
	*/
	void draw(const std::shared_ptr<CRenderer>& renderer, const CCamera& camera, bool culling = true);

private:
	/**
	* \brief Checks culling state for scene object.
	* Always returns false if d_culling is set to false
	*/
	bool isCulled(const CRenderable* entity, const CCamera& camera) const;

	std::list<std::shared_ptr<CRenderable>> d_objects; /**< Current objects in scene. */
	glm::vec3 d_backgroundColor; /**< Scene background color. */
};
