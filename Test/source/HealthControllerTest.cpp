#include "game/control/HealthController.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"
#include "collision/CollisionSystem.h"
#include "game/GameObject.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("HealthController ignores objects without a collidable", "[health]")
{
	GameObject object;
	HealthController controller(10.f);
	controller.attach(&object);

	controller.update(0.f);

	CHECK_FALSE(object.isDead());
}

TEST_CASE("HealthController keeps an object alive while damage is sub-lethal", "[health]")
{
	// Must outlive the GameObject, whose destructor marks the attached one as deleted.
	Collidable collidable(0, AABBox());
	GameObject object;
	object.setCollidable(&collidable);

	HealthController controller(10.f);
	controller.attach(&object);

	collidable.receiveDamage(4.f);
	controller.update(1.f);
	CHECK_FALSE(object.isDead());

	collidable.receiveDamage(3.f);
	controller.update(1.f);
	CHECK_FALSE(object.isDead());
}

TEST_CASE("HealthController kills an object once damage reaches its health", "[health]")
{
	// Must outlive the GameObject, whose destructor marks the attached one as deleted.
	Collidable collidable(0, AABBox());
	GameObject object;
	object.setCollidable(&collidable);

	HealthController controller(10.f);
	controller.attach(&object);

	collidable.receiveDamage(10.f);
	controller.update(1.f);
	CHECK(object.isDead());
}

TEST_CASE("HealthController consumes accumulated damage only once", "[health]")
{
	// Must outlive the GameObject, whose destructor marks the attached one as deleted.
	Collidable collidable(0, AABBox());
	GameObject object;
	object.setCollidable(&collidable);

	HealthController controller(10.f);
	controller.attach(&object);

	collidable.receiveDamage(6.f);
	controller.update(1.f);
	REQUIRE_FALSE(object.isDead());

	// Only the 3 points just received count; one that did not drain would subtract 9 and kill.
	collidable.receiveDamage(3.f);
	controller.update(1.f);
	CHECK_FALSE(object.isDead());
}
TEST_CASE("A single contact kills a target whose health is below the damage", "[health]")
{
	// Shipped numbers: player deals 30 against 25 health, projectiles 40.
	CollisionSystem system;
	const unsigned int playerGroup = system.getNewGroupId();
	const unsigned int enemyGroup = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	GameObject playerObject;
	playerObject.setCollidable(system.add(box, playerGroup));
	playerObject.getCollidable()->setDamage(30.f);

	GameObject enemyObject;
	enemyObject.setCollidable(system.add(box, enemyGroup));
	HealthController enemyHealth(25.f);
	enemyHealth.attach(&enemyObject);

	system.update();
	enemyHealth.update(1.f);

	CHECK(enemyObject.isDead());
}
