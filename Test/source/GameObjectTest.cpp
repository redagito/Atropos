#include "game/GameObject.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GameObject can clear its collidable", "[gameobject]")
{
	GameObject object;
	Collidable collidable(0, AABBox());

	object.setCollidable(&collidable);
	REQUIRE(object.getCollidable() == &collidable);

	object.setCollidable(nullptr);
	CHECK(object.getCollidable() == nullptr);
}
