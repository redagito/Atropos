#include "collision/CollisionSystem.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("CollisionSystem ignores removing a null entity", "[collision]")
{
	CollisionSystem system;

	CHECK_NOTHROW(system.remove(nullptr));
}

TEST_CASE("CollisionSystem defers entity removal to update", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();
	Collidable* entity = system.add(AABBox(), group);

	system.remove(entity);

	REQUIRE(entity->deleteRequested());
	CHECK_NOTHROW(system.update());
}

TEST_CASE("CollisionSystem deals damage between colliding groups", "[collision]")
{
	CollisionSystem system;
	const unsigned int groupA = system.getNewGroupId();
	const unsigned int groupB = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	Collidable* entityA = system.add(box, groupA);
	Collidable* entityB = system.add(box, groupB);
	entityA->setDamage(5.f);

	system.update();

	CHECK(entityB->getDamageReceived() == 5.f);
	CHECK(entityA->getDamageReceived() == 0.f);
}

TEST_CASE("CollisionSystem reuses released group ids", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();
	system.add(AABBox(), group);

	system.releaseGroup(group);

	CHECK(system.getNewGroupId() == group);
}

TEST_CASE("CollisionSystem ignores invalid or repeated group releases", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();

	CHECK_NOTHROW(system.releaseGroup(999));
	system.releaseGroup(group);
	CHECK_NOTHROW(system.releaseGroup(group));

	// The id is handed back out only once
	CHECK(system.getNewGroupId() == group);
	CHECK(system.getNewGroupId() == group + 1);
}
