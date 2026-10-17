#include "renderer/scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("Scene background defaults to white", "[scene]")
{
	Scene scene;

	CHECK(scene.getBackgroundColor() == glm::vec3(1.f));
}

TEST_CASE("Scene background color can be changed", "[scene]")
{
	Scene scene;
	const glm::vec3 color(0.1f, 0.2f, 0.3f);

	scene.setBackgroundColor(color);

	CHECK(scene.getBackgroundColor() == color);
}