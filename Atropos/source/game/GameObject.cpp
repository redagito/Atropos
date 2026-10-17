#include "game/GameObject.h"

#include "game/IController.h"

#include "renderer/Renderable.h"
#include "collision/Collidable.h"

#include <cassert>

// Constructor initializes drawable and matrices
GameObject::GameObject()
	:
	d_translation(glm::mat4(1.f)), // Transformation matrices
	d_rotation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_transformationChanged(false), // Dirty flag for lazy update
	d_drawable(nullptr), // Drawable
	d_collidable(nullptr), // Collidable
	d_deleteRequested(false), // Delete request flag
	d_dead(false) // Object is initially alive
{
	return;
}

GameObject::~GameObject()
{
	// Send detach event to controllers
	for (const auto& controller : d_controllers)
	{
		controller->detach();
	}

	// Delete drawable
	if (d_drawable != nullptr)
	{
		d_drawable->markDeleted();
		d_drawable = nullptr;
	}
	
	// Delete collidable
	if (d_collidable != nullptr)
	{
		d_collidable->markDeleted();
		d_collidable = nullptr;
	}
}

void GameObject::update(float dtime)
{
	if (d_transformationChanged)
	{
		d_transformationChanged = false;
		// Update collidable
		if (d_collidable != nullptr)
		{
			d_collidable->setScale(d_scale);
			d_collidable->setTranslation(d_translation);
		}

		// Update drawable
		if (d_drawable)
		{
			d_drawable->setRotationMatrix(d_rotation);
			d_drawable->setTranslationMatrix(d_translation);
			d_drawable->setScaleMatrix(d_scale);
			d_drawable->setModelMatrix(d_translation * d_scale * d_rotation);
		}
	}

	// Update state with attached controllers
	for (const auto& controller : d_controllers)
	{
		controller->update(dtime);
	}
}

void GameObject::setDrawable(Renderable* proxy)
{
	if (d_drawable != proxy)
	{
		// Release the previously attached drawable
		if (d_drawable != nullptr)
		{
			d_drawable->markDeleted();
		}
		d_drawable = proxy;
		// Update
		if (d_drawable != nullptr)
		{
			d_drawable->setRotationMatrix(d_rotation);
			d_drawable->setTranslationMatrix(d_translation);
			d_drawable->setScaleMatrix(d_scale);
			d_drawable->setModelMatrix(d_translation * d_scale * d_rotation);
		}
	}
}

Renderable* GameObject::getDrawable() const
{
	return d_drawable;
}

void GameObject::setCollidable(Collidable* entity)
{
	if (d_collidable != entity)
	{
		// Release the previously attached collidable
		if (d_collidable != nullptr)
		{
			d_collidable->markDeleted();
		}
		d_collidable = entity;
		// Update
		if (d_collidable != nullptr)
		{
			d_collidable->setScale(d_scale);
			d_collidable->setTranslation(d_translation);
		}
	}
}

Collidable* GameObject::getCollidable() const
{
	return d_collidable;
}

void GameObject::addController(const std::shared_ptr<IController>& controller)
{
	assert(controller != nullptr);
	controller->attach(this);
	d_controllers.push_back(controller);
}

void GameObject::setRotation(const glm::mat4& rotation)
{
	d_rotation = rotation;
	d_transformationChanged = true;
}

void GameObject::setTranslation(const glm::mat4& translation)
{
	d_translation = translation;
	d_transformationChanged = true;
}

void GameObject::setScale(const glm::mat4& scale)
{
	d_scale = scale;
	d_transformationChanged = true;
}

const glm::mat4& GameObject::getTranslation() const
{
	return d_translation;
}

void GameObject::markDeleted()
{
	d_deleteRequested = true;
}

bool GameObject::isDeleteRequested() const
{
	return d_deleteRequested;
}

bool GameObject::isDead() const
{
	return d_dead;
}

void GameObject::setDead()
{
	d_dead = true;
}
