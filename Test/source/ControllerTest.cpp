#include "game/control/LinearMovementController.h"
#include "game/control/ShiftController.h"

#include "game/GameObject.h"
#include "renderer/scene/Camera.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("LinearMovementController moves its attached object", "[controller]")
{
	LinearMovementController controller(glm::vec3(1.f, 0.f, 0.f));
	controller.setActive(true);

	GameObject object;
	controller.attach(&object);
	controller.update(1.f);

	CHECK(object.getTranslation() != glm::mat4(1.f));
}

TEST_CASE("LinearMovementController ignores updates while detached", "[controller]")
{
	LinearMovementController controller(glm::vec3(1.f, 0.f, 0.f));
	controller.setActive(true);

	GameObject object;
	const glm::mat4 before = object.getTranslation();

	controller.detach();
	controller.update(1.f);

	CHECK(object.getTranslation() == before);
}

TEST_CASE("ShiftController ignores updates while detached", "[controller]")
{
	Camera camera;
	ShiftController controller(&camera, 10.f);
	controller.setActive(true);

	GameObject object;
	const glm::mat4 before = object.getTranslation();

	controller.detach();
	controller.update(1.f);

	CHECK(object.getTranslation() == before);
}
