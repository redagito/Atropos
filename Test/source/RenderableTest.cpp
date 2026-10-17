#include "renderer/Renderable.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

TEST_CASE("Renderable tolerates a missing model", "[renderable]")
{
	Renderable renderable(nullptr);

	CHECK_NOTHROW(renderable.setTranslationMatrix(glm::mat4(1.f)));
	CHECK_NOTHROW(renderable.setScaleMatrix(glm::mat4(1.f)));

	CHECK(renderable.getAABBox().getMid() == glm::vec3(0.f));
	CHECK(renderable.getAABBox().getHalfWidths() == glm::vec3(0.f));
}
