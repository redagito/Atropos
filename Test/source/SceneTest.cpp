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

TEST_CASE("A scene keeps the sky to itself", "[scene]")
{
	Scene play;
	Scene win;

	// Off by default, so a scene that never asks for a sky cannot inherit one that
	// another scene switched on and left behind.
	CHECK(play.isSkyEnabled() == false);

	play.setSky(true, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 0.f));

	CHECK(play.isSkyEnabled() == true);
	CHECK(win.isSkyEnabled() == false);
}