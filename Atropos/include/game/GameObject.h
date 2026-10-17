#pragma once

// Forward declare
class Renderable;
class Collidable;
class IController;

#include <glm/glm.hpp>

#include <list>
#include <memory>

/**
* \brief Abstract game object storing relevant data.
*/
class GameObject
{
public:
	/**
	* \brief Default constructor.
	*/
	GameObject();

	/**
	* \brief Resource cleanup.
	*/
	~GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	/**
	* \brief Updates object and returns state
	*
	* Updates the object with time in seconds.
	*/
	void update(float dtime);

	/**
	* \brief Sets the graphical representation of the object.
	*/
	void setDrawable(Renderable* entity);

	/**
	* \brief Returns renderable.
	*/
	Renderable* getDrawable() const;

	/**
	* \brief Sets the collision entity for the object.
	*/
	void setCollidable(Collidable* entity);
	
	/**
	* \brief Returns collidable entity.
	*/
	Collidable* getCollidable() const;

	/**
	* \brief Adds controller to object.
	*/
	void addController(const std::shared_ptr<IController>& controller);

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
	* \brief Returns translation matrix
	*/
	const glm::mat4& getTranslation() const;

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
	Renderable* d_drawable; /**< Drawable object. */
	Collidable* d_collidable; /**< Collidable object. */

	// Game object state
	bool d_dead; /**< Death flag. */

	std::list<std::shared_ptr<IController>> d_controllers; /**< Controller objects. */
	
	bool d_transformationChanged; /**< Dirty flag for tranformation matrices, set by transformation setter call. */
	bool d_deleteRequested; /**< Deletion of this object is requested. */
};
