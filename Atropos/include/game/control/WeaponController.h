#pragma once

class IRenderSystem;
class ISoundSystem;
class CollisionSystem;
class GameWorld;

#include "game/IController.h"

#include <string>

class WeaponController : public IController
{
public:
	WeaponController(IRenderSystem* renderSys, ISoundSystem* soundSys, CollisionSystem* collSys, GameWorld* world, const std::string& soundName, float volume, const std::string& modelName, unsigned int collisionGroup, float cooldown, float damage);

	~WeaponController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	IRenderSystem* d_renderSys;
	ISoundSystem* d_soundSys;
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
