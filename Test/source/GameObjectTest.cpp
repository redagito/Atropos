#include "game/GameObject.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"
#include "renderer/Renderable.h"

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

TEST_CASE("GameObject marks a replaced collidable for deletion", "[gameobject]")
{
	GameObject object;
	Collidable first(0, AABBox());
	Collidable second(0, AABBox());

	object.setCollidable(&first);
	object.setCollidable(&second);

	CHECK(first.deleteRequested());
	CHECK(object.getCollidable() == &second);
}

TEST_CASE("GameObject marks a replaced drawable for deletion", "[gameobject]")
{
	GameObject object;
	Renderable first(nullptr);
	Renderable second(nullptr);

	object.setDrawable(&first);
	object.setDrawable(&second);

	CHECK(first.deleteRequested());
	CHECK(object.getDrawable() == &second);
}
