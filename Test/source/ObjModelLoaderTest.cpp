#include "io/ObjModelLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

namespace
{
	bool loadFromString(const std::string& contents, ObjModelLoader& loader)
	{
		static std::mt19937 generator(std::random_device{}());
		const std::filesystem::path path = std::filesystem::temp_directory_path()
			/ ("atropos_obj_" + std::to_string(generator()) + ".obj");

		{
			std::ofstream out(path);
			out << contents;
		}

		std::string fileName = path.string();
		const bool result = loader.load(fileName);
		std::filesystem::remove(path);
		return result;
	}
}

TEST_CASE("ObjModelLoader loads a fully specified triangle", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"vt 0 0\n"
		"vt 1 0\n"
		"vt 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"f 1/1/1 2/2/2 3/3/3\n";

	ObjModelLoader loader;
	REQUIRE(loadFromString(contents, loader));

	REQUIRE(loader.getVertices().size() == 9);
	REQUIRE(loader.getNormals().size() == 9);
	REQUIRE(loader.getUV().size() == 6);

	CHECK(loader.getVertices()[0] == 0.f);
	CHECK(loader.getVertices()[1] == 0.f);
	CHECK(loader.getVertices()[2] == 0.f);
	CHECK(loader.getVertices()[3] == 1.f);
	CHECK(loader.getVertices()[4] == 0.f);
	CHECK(loader.getVertices()[5] == 0.f);
	CHECK(loader.getVertices()[6] == 0.f);
	CHECK(loader.getVertices()[7] == 1.f);
	CHECK(loader.getVertices()[8] == 0.f);

	CHECK(loader.getNormals()[0] == 0.f);
	CHECK(loader.getNormals()[1] == 0.f);
	CHECK(loader.getNormals()[2] == 1.f);

	CHECK(loader.getUV()[0] == 0.f);
	CHECK(loader.getUV()[1] == 0.f);
	CHECK(loader.getUV()[2] == 1.f);
	CHECK(loader.getUV()[3] == 0.f);
	CHECK(loader.getUV()[4] == 0.f);
	CHECK(loader.getUV()[5] == 1.f);
}

TEST_CASE("ObjModelLoader supports negative indices", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"vt 0 0\n"
		"vt 1 0\n"
		"vt 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"f -3/-3/-3 -2/-2/-2 -1/-1/-1\n";

	ObjModelLoader loader;
	REQUIRE(loadFromString(contents, loader));

	REQUIRE(loader.getVertices().size() == 9);
	REQUIRE(loader.getNormals().size() == 9);
	REQUIRE(loader.getUV().size() == 6);

	CHECK(loader.getVertices()[3] == 1.f);
	CHECK(loader.getVertices()[7] == 1.f);
	CHECK(loader.getNormals()[2] == 1.f);
	CHECK(loader.getUV()[2] == 1.f);
	CHECK(loader.getUV()[5] == 1.f);
}

TEST_CASE("ObjModelLoader handles normals-only faces", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"f 1//1 2//2 3//3\n";

	ObjModelLoader loader;
	REQUIRE(loadFromString(contents, loader));

	REQUIRE(loader.getVertices().size() == 9);
	REQUIRE(loader.getNormals().size() == 9);
	REQUIRE(loader.getUV().size() == 6);

	CHECK(loader.getVertices()[3] == 1.f);
	CHECK(loader.getNormals()[2] == 1.f);
	for (float value : loader.getUV())
	{
		CHECK(value == 0.f);
	}
}

TEST_CASE("ObjModelLoader handles position-uv faces", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"vt 0 0\n"
		"vt 1 0\n"
		"vt 0 1\n"
		"f 1/1 2/2 3/3\n";

	ObjModelLoader loader;
	REQUIRE(loadFromString(contents, loader));

	REQUIRE(loader.getVertices().size() == 9);
	REQUIRE(loader.getNormals().size() == 9);
	REQUIRE(loader.getUV().size() == 6);

	CHECK(loader.getVertices()[3] == 1.f);
	CHECK(loader.getUV()[2] == 1.f);
	for (float value : loader.getNormals())
	{
		CHECK(value == 0.f);
	}
}

TEST_CASE("ObjModelLoader handles position-only faces", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"f 1 2 3\n";

	ObjModelLoader loader;
	REQUIRE(loadFromString(contents, loader));

	REQUIRE(loader.getVertices().size() == 9);
	REQUIRE(loader.getNormals().size() == 9);
	REQUIRE(loader.getUV().size() == 6);

	CHECK(loader.getVertices()[3] == 1.f);
	for (float value : loader.getUV())
	{
		CHECK(value == 0.f);
	}
	for (float value : loader.getNormals())
	{
		CHECK(value == 0.f);
	}
}

TEST_CASE("ObjModelLoader skips out-of-range face indices", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"f 99/99/99 100/100/100 101/101/101\n";

	ObjModelLoader loader;
	CHECK(loadFromString(contents, loader));
	CHECK(loader.getVertices().empty());
}

TEST_CASE("ObjModelLoader ignores malformed declaration lines", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"v\n"
		"vt\n"
		"vn\n"
		"f\n"
		"f 1 2 3\n";

	ObjModelLoader loader;
	CHECK(loadFromString(contents, loader));
	CHECK(loader.getVertices().size() == 9);
}

TEST_CASE("ObjModelLoader handles a vertex-less model", "[obj]")
{
	const std::string contents = "# empty model\n";

	ObjModelLoader loader;
	CHECK(loadFromString(contents, loader));
	CHECK(loader.getVertices().empty());
}
