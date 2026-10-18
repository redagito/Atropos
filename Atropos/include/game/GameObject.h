#pragma once

class Renderable;
class Collidable;
class IController;

#include <glm/glm.hpp>

#include <list>
#include <memory>

class GameObject
{
public:
	GameObject();

	~GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	// dtime is in seconds.
	void update(float dtime);

	void setDrawable(Renderable* entity);

	Renderable* getDrawable() const;

	void setCollidable(Collidable* entity);
	
	Collidable* getCollidable() const;

	void addController(const std::shared_ptr<IController>& controller);

	void setRotation(const glm::mat4& rotation);

	void setTranslation(const glm::mat4& translation);

	void setScale(const glm::mat4& scale);

	const glm::mat4& getTranslation() const;

	void markDeleted();

	bool isDeleteRequested() const;

	bool isDead() const;

	void setDead();

protected:
	glm::mat4 d_rotation; /**< Rotation matrix. */
	glm::mat4 d_translation; /**< Translation matrix. */
	glm::mat4 d_scale; /**< Scale matrix. */

	// TODO These should actually be components and the game object should be an entity
	Renderable* d_drawable; /**< Drawable object. */
	Collidable* d_collidable; /**< Collidable object. */

	bool d_dead; /**< Death flag. */

	std::list<std::shared_ptr<IController>> d_controllers; /**< Controller objects. */
	
	bool d_transformationChanged; /**< Dirty flag for tranformation matrices, set by transformation setter call. */
	bool d_deleteRequested; /**< Deletion of this object is requested. */
};
