#pragma once

// Forward declare
class CRenderable;
class CCollidable;
class IController;

#include <glm/glm.hpp>

#include <list>
#include <memory>

/**
* \brief Abstract game object storing relevant data.
*/
class CGameObject
{
public:
	/**
	* \brief Default constructor.
	*/
	CGameObject();

	/**
	* \brief Resource cleanup.
	*/
	~CGameObject();

	/**
	* \brief Updates object and returns state
	*
	* Updates the object with time in seconds.
	*/
	void update(float dtime);

	/**
	* \brief Sets the graphical representation of the object.
	*/
	void setDrawable(CRenderable* entity);

	/**
	* \brief Returns renderable.
	*/
	CRenderable* getDrawable() const;

	/**
	* \brief Sets the collision entity for the object.
	*/
	void setCollidable(CCollidable* entity);
	
	/**
	* \brief Returns collidable entity.
	*/
	CCollidable* getCollidable() const;

	/**
	* \brief Adds controller to object.
	*/
	void addController(const std::shared_ptr<IController>& controller);

	/**
	 * \brief Removes controller from object.
	 */
	void removeController(const std::shared_ptr<IController>& controller);

	/**
	* \brief Sets rotation matrix.
	*/
	void setRotation(const glm::mat4& rotation);

	/**
	* \brief Sets translation matrix.
	*/
	void setTranslation(const glm::mat4& translation);

	/**
	* \brief Sets scale matrix.
	*/
	void setScale(const glm::mat4& scale);

	/**
	* \brief Returns rotation matrix
	*/
	const glm::mat4& getRotation() const;

	/**
	* \brief Returns translation matrix
	*/
	const glm::mat4& getTranslation() const;
	
	/**
	* \brief Returns scale matrix
	*/
	const glm::mat4& getScale() const;

	/**
	* \brief Marks this object for deletion.
	*/
	void markDeleted();

	/**
	* \brief Returns delete request flag.
	* True if object should be deleted.
	*/
	bool isDeleteRequested() const;

	/**
	 * \brief Is the object considered to be dead.
	 */
	bool isDead() const;

	/**
	 * \brief Sets the object into death state.
	 */
	void setDead();

protected:
	glm::mat4 d_rotation; /**< Rotation matrix. */
	glm::mat4 d_translation; /**< Translation matrix. */
	glm::mat4 d_scale; /**< Scale matrix. */

	// TODO These should actually be components and the game object should be an entity
	CRenderable* d_drawable; /**< Drawable object. */
	CCollidable* d_collidable; /**< Collidable object. */

	// Game object state
	bool d_dead; /**< Death flag. */

	std::list<std::shared_ptr<IController>> d_controllers; /**< Controller objects. */
	
	bool d_transformationChanged; /**< Dirty flag for tranformation matrices, set by transformation setter call. */
	bool d_deleteRequested; /**< Deletion of this object is requested. */
};
