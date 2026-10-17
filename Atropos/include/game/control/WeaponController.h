#pragma once

class RenderSystem;
class SoundSystem;
class CollisionSystem;
class GameWorld;

#include "game/IController.h"

#include <string>

/**
* \brief Weapon controller creates projectiles and sound effects.
*/
class WeaponController : public IController
{
public:
	/**
	* \brief .
	*/
	WeaponController(RenderSystem* renderSys, SoundSystem* soundSys, CollisionSystem* collSys, GameWorld* world, const std::string& soundName, float volume, const std::string& modelName, unsigned int collisionGroup, float cooldown, float damage);

	/**
	* \brief Destructor for cleanup.
	*/
	~WeaponController();

	/**
	* \brief On attach to object.
	*/
	void attach(GameObject* object);

	/**
	* \brief On detaching from game object.
	*/
	void detach();

	/**
	* \brief Updates controller.
	*/
	void update(float dtime);

	/**
	* \brief Sets active state.
	*/
	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	RenderSystem* d_renderSys;
	SoundSystem* d_soundSys;
	CollisionSystem* d_collSys;
	GameWorld* d_world;
	std::string d_soundName;
	std::string d_modelName;
	float d_damage;
	float d_volume;
	float d_cooldown;
	float d_currentCooldown;
	unsigned int d_collisionGroup;
	bool d_active; /**< Active state flag. */
};
