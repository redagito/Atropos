#include "CGameObject.h"

#include "IController.h"

#include "renderer/CRenderable.h"
#include "collision/CCollidable.h"

#include <cassert>

// Constructor initializes drawable and matrices
CGameObject::CGameObject()
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

CGameObject::~CGameObject()
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

void CGameObject::update(float dtime)
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

void CGameObject::setDrawable(CRenderable* proxy)
{
	if (d_drawable != proxy)
	{
		d_drawable = proxy;
		// Update
		d_drawable->setRotationMatrix(d_rotation);
		d_drawable->setTranslationMatrix(d_translation);
		d_drawable->setScaleMatrix(d_scale);
		d_drawable->setModelMatrix(d_translation * d_scale * d_rotation);
	}
}

CRenderable* CGameObject::getDrawable() const
{
	return d_drawable;
}

void CGameObject::setCollidable(CCollidable* entity)
{
	if (d_collidable != entity)
	{
		d_collidable = entity;
		// Update
		d_collidable->setScale(d_scale);
		d_collidable->setTranslation(d_translation);
	}
}

CCollidable* CGameObject::getCollidable() const
{
	return d_collidable;
}

void CGameObject::addController(const std::shared_ptr<IController>& controller)
{
	assert(controller != nullptr);
	controller->attach(this);
	d_controllers.push_back(controller);
}

void CGameObject::removeController(const std::shared_ptr<IController>& controller)
{
	controller->detach();
	d_controllers.remove(controller);
}

void CGameObject::setRotation(const glm::mat4& rotation)
{
	d_rotation = rotation;
	d_transformationChanged = true;
}

void CGameObject::setTranslation(const glm::mat4& translation)
{
	d_translation = translation;
	d_transformationChanged = true;
}

void CGameObject::setScale(const glm::mat4& scale)
{
	d_scale = scale;
	d_transformationChanged = true;
}

const glm::mat4& CGameObject::getRotation() const
{
	return d_rotation;
}

const glm::mat4& CGameObject::getTranslation() const
{
	return d_translation;
}

const glm::mat4& CGameObject::getScale() const
{
	return d_scale;
}

void CGameObject::markDeleted()
{
	d_deleteRequested = true;
}

bool CGameObject::isDeleteRequested() const
{
	return d_deleteRequested;
}

bool CGameObject::isDead() const
{
	return d_dead;
}

void CGameObject::setDead()
{
	d_dead = true;
}
