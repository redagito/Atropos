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

	// The menu screen needs the default framing back after a run moved the camera.
	CHECK(camera.getViewMatrix() == defaultView);
	CHECK(camera.getPosition() == defaultPosition);
}