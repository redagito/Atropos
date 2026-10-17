#include "game/MenuShip.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MenuShip does nothing before a model is shown", "[menuship]")
{
	MenuShip ship;

	// A state calls update before the display has been created, so this has to
	// be a no-op. With an uninitialised pointer it dereferenced garbage and
	// took the process down with an access violation.
	CHECK_NOTHROW(ship.update(1.f));
	CHECK_NOTHROW(ship.update(0.f));
}