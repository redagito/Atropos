#include "collision/CollisionSystem.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("CollisionSystem ignores removing a null entity", "[collision]")
{
	CollisionSystem system;

	CHECK_NOTHROW(system.remove(nullptr));
}

TEST_CASE("CollisionSystem removes an added entity", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();
	Collidable* entity = system.add(AABBox(), group);

	CHECK_NOTHROW(system.remove(entity));
}
