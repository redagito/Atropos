#include "game/GameObject.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"
#include "game/control/LinearMovementController.h"
#include "renderer/Renderable.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("GameObject can clear its collidable", "[gameobject]")
{
	GameObject object;
	Collidable collidable(0, AABBox());

	object.setCollidable(&collidable);
	REQUIRE(object.getCollidable() == &collidable);

	object.setCollidable(nullptr);
	CHECK(object.getCollidable() == nullptr);
}

TEST_CASE("GameObject moves its collidable along with its controllers", "[gameobject]")
{
	// The collidable must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so it is declared first.
	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));
	Collidable collidable(0, box);
	GameObject object;
	object.setCollidable(&collidable);

	auto controller = std::make_shared<LinearMovementController>(glm::vec3(0.f, 0.f, 10.f));
	controller->setActive(true);
	object.addController(controller);

	object.update(1.f);

	// The box has to describe where the object ended the frame. Flushing the
	// transformation before the controllers run leaves every collision box one
	// full step behind its owner, which lets fast objects pass through others.
	CHECK(glm::vec3(collidable.getAABBox().getMid()) == glm::vec3(0.f, 0.f, 10.f));
}

TEST_CASE("GameObject marks a replaced collidable for deletion", "[gameobject]")
{
	// The collidables must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so they are declared first.
	Collidable first(0, AABBox());
	Collidable second(0, AABBox());
	GameObject object;

	object.setCollidable(&first);
	object.setCollidable(&second);

	CHECK(first.deleteRequested());
	CHECK(object.getCollidable() == &second);
}

TEST_CASE("GameObject marks a replaced drawable for deletion", "[gameobject]")
{
	// The renderables must outlive the GameObject, whose destructor marks the
	// attached one as deleted, so they are declared first.
	Renderable first(nullptr);
	Renderable second(nullptr);
	GameObject object;

	object.setDrawable(&first);
	object.setDrawable(&second);

	CHECK(first.deleteRequested());
	CHECK(object.getDrawable() == &second);
}
