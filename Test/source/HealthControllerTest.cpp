#include "game/control/HealthController.h"

#include "game/GameObject.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("HealthController ignores objects without a collidable", "[health]")
{
	GameObject object;
	HealthController controller(10.f);
	controller.attach(&object);

	controller.update(0.f);

	CHECK_FALSE(object.isDead());
}
