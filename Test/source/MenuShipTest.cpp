#include "game/MenuShip.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MenuShip does nothing before a model is shown", "[menuship]")
{
	MenuShip ship;

	// update runs before the display exists, so it must be a no-op, not a null dereference.
	CHECK_NOTHROW(ship.update(1.f));
	CHECK_NOTHROW(ship.update(0.f));
}