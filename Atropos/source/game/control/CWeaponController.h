#pragma once

class CRenderSystem;
class CSoundSystem;
class CCollisionSystem;
class CGameWorld;

#include "game/IController.h"

#include <string>

/**
* \brief Weapon controller creates projectiles and sound effects.
*/
class CWeaponController : public IController
{
public:
	/**
	* \brief .
	*/
	CWeaponController(CRenderSystem* renderSys, CSoundSystem* soundSys, CCollisionSystem* collSys, CGameWorld* world, const std::string& soundName, float volume, const std::string& modelName, unsigned int collisionGroup, float cooldown, float damage);

	/**
	* \brief Destructor for cleanup.
	*/
	~CWeaponController();

	/**
	* \brief On attach to object.
	*/
	void attach(CGameObject* object);

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
	CGameObject* d_object; /**< Controlled game object. */
	CRenderSystem* d_renderSys;
	CSoundSystem* d_soundSys;
	CCollisionSystem* d_collSys;
	CGameWorld* d_world;
	std::string d_soundName;
	std::string d_modelName;
	float d_damage;
	float d_volume;
	float d_cooldown;
	float d_currentCooldown;
	unsigned int d_collisionGroup;
	bool d_active; /**< Active state flag. */
};
