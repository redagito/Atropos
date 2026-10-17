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
	// The collidable must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so it is declared first.
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
	// The collidable must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so it is declared first.
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
	// The collidable must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so it is declared first.
	Collidable collidable(0, AABBox());
	GameObject object;
	object.setCollidable(&collidable);

	HealthController controller(10.f);
	controller.attach(&object);

	collidable.receiveDamage(6.f);
	controller.update(1.f);
	REQUIRE_FALSE(object.isDead());

	// The accumulator was drained by the first update, so this update only
	// sees the 3 points just received and 10 - 6 - 3 leaves the object alive.
	// A controller that did not drain it would subtract 9 and kill the object.
	collidable.receiveDamage(3.f);
	controller.update(1.f);
	CHECK_FALSE(object.isDead());
}
TEST_CASE("A single contact kills a target whose health is below the damage", "[health]")
{
	// These are the numbers the game ships with: the player deals 30 damage and
	// an enemy has 25 health, so ramming one kills it on contact. A projectile
	// deals 40, which also kills in one hit.
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
