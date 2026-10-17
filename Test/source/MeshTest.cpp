#include "renderer/resource/Mesh.h"

#include "collision/AABBox.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Mesh init rejects empty vertex data", "[mesh]")
{
	Mesh mesh;

	CHECK_THROWS(mesh.init({}, {}, {}, AABBox()));
}
