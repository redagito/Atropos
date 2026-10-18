#include "game/GameObject.h"
#include "game/control/HealthController.h"
#include "game/control/ScoreOnDeathController.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("A score controller counts a death exactly once", "[score]")
{
	int score = 0;
	GameObject object;
	ScoreOnDeathController controller(&score);
	controller.attach(&object);

	controller.update(0.016f);
	CHECK(score == 0);

	object.setDead();
	controller.update(0.016f);
	CHECK(score == 1);

	// A second update must not count the same death again.
	controller.update(0.016f);
	CHECK(score == 1);
}

TEST_CASE("A score controller ignores an inactive object", "[score]")
{
	int score = 0;
	GameObject object;
	ScoreOnDeathController controller(&score);
	controller.attach(&object);
	object.setDead();

	controller.setActive(false);
	controller.update(0.016f);
	CHECK(score == 0);

	controller.setActive(true);
	controller.update(0.016f);
	CHECK(score == 1);
}

TEST_CASE("A score controller survives a missing score target", "[score]")
{
	GameObject object;
	ScoreOnDeathController controller(nullptr);
	controller.attach(&object);
	object.setDead();

	controller.update(0.016f);
	CHECK(true);
}

TEST_CASE("Health reports the damage taken and what it started at", "[health]")
{
	// Must outlive the GameObject, whose destructor marks the attached one as deleted.
	Collidable collidable(0, AABBox());
	GameObject object;
	object.setCollidable(&collidable);

	HealthController health(100.f);
	health.attach(&object);

	CHECK(health.getMaxHealth() == 100.f);
	CHECK(health.getHealth() == 100.f);

	collidable.receiveDamage(25.f);
	health.update(0.016f);

	CHECK(health.getHealth() == 75.f);
	CHECK(object.isDead() == false);

	collidable.receiveDamage(100.f);
	health.update(0.016f);

	CHECK(health.getHealth() <= 0.f);
	CHECK(object.isDead() == true);
}