#include "renderer/scene/Camera.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("Camera resets its view to the default", "[camera]")
{
	Camera camera;
	const glm::mat4 defaultView = camera.getViewMatrix();
	const glm::vec3 defaultPosition = camera.getPosition();

	camera.setViewMatrix(glm::vec3(1.f, 2.f, 3.f), glm::vec3(4.f, 5.f, 6.f), glm::vec3(0.f, 1.f, 0.f));
	REQUIRE(camera.getViewMatrix() != defaultView);

	camera.resetView();

	// A menu screen shows a model at a fixed place in the world, so the camera
	// has to be able to go back to the default framing after a run left it
	// thousands of units away with the player.
	CHECK(camera.getViewMatrix() == defaultView);
	CHECK(camera.getPosition() == defaultPosition);
}