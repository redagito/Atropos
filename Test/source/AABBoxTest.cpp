#include "collision/AABBox.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

#include <memory>

TEST_CASE("AABBox starts at the origin with zero halfwidths", "[aabb]")
{
	// Allocate on the heap so the members are not zero-filled by the stack.
	auto box = std::make_unique<AABBox>();

	REQUIRE(box->getMid() == glm::vec3(0.f));
	REQUIRE(box->getHalfWidths() == glm::vec3(0.f));
}

TEST_CASE("AABBox stores its middle point and halfwidths", "[aabb]")
{
	AABBox box;
	box.setMid(glm::vec3(1.f, 2.f, 3.f));
	box.setHalfWidths(glm::vec3(4.f, 5.f, 6.f));

	CHECK(box.getMid() == glm::vec3(1.f, 2.f, 3.f));
	CHECK(box.getHalfWidths() == glm::vec3(4.f, 5.f, 6.f));
}
