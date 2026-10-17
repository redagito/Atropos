#include "game/control/LinearMovementController.h"
#include "game/control/ShiftController.h"

#include "renderer/scene/Camera.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("LinearMovementController update without an attached object is safe", "[controller]")
{
	LinearMovementController controller(glm::vec3(1.f, 0.f, 0.f));
	controller.setActive(true);

	controller.update(0.1f);
}

TEST_CASE("ShiftController update without an attached object is safe", "[controller]")
{
	Camera camera;
	ShiftController controller(&camera, 10.f);
	controller.setActive(true);

	controller.update(0.1f);
}
