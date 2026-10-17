#include "io/ObjModelLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <process.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace
{
	void loadFromString(const std::string& contents, ObjModelLoader& loader)
	{
		// ctest gives every test case its own process, so the counter restarts
		// at zero everywhere and would collide across concurrent runs. The
		// process id makes the name unique among simultaneously running tests.
		static int counter = 0;
		const std::filesystem::path path = std::filesystem::temp_directory_path()
			/ ("atropos_obj_" + std::to_string(_getpid()) + "_" + std::to_string(counter++) + ".obj");

		{
			std::ofstream out(path);
			out << contents;
		}

		std::string fileName = path.string();
		try
		{
			loader.load(fileName);
		}
		catch (...)
		{
			std::filesystem::remove(path);
			throw;
		}
		std::filesystem::remove(path);
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
	REQUIRE_NOTHROW(loadFromString(contents, loader));

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
	REQUIRE_NOTHROW(loadFromString(contents, loader));

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
	REQUIRE_NOTHROW(loadFromString(contents, loader));

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
	REQUIRE_NOTHROW(loadFromString(contents, loader));

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
	REQUIRE_NOTHROW(loadFromString(contents, loader));

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
	// Every face index is out of range, so no geometry is produced and the
	// load is rejected.
	CHECK_THROWS(loadFromString(contents, loader));
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
	CHECK_NOTHROW(loadFromString(contents, loader));
	CHECK(loader.getVertices().size() == 9);
}

TEST_CASE("ObjModelLoader ignores lines with missing float values", "[obj]")
{
	const std::string contents =
		"v 1 2\n"
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"vt 0.5\n"
		"vt 1.5 2.5\n"
		"vn 1 2\n"
		"vn 0 0 1\n"
		"f 1/1/1 2/1/1 3/1/1\n";

	ObjModelLoader loader;
	REQUIRE_NOTHROW(loadFromString(contents, loader));

	// The malformed "v 1 2" must not shift the vertex indices.
	REQUIRE(loader.getVertices().size() == 9);
	CHECK(loader.getVertices()[0] == 0.f);
	CHECK(loader.getVertices()[1] == 0.f);
	CHECK(loader.getVertices()[2] == 0.f);
	CHECK(loader.getVertices()[3] == 1.f);

	REQUIRE(loader.getUV().size() == 6);
	CHECK(loader.getUV()[0] == 1.5f);
	CHECK(loader.getUV()[1] == 2.5f);

	REQUIRE(loader.getNormals().size() == 9);
	CHECK(loader.getNormals()[2] == 1.f);
}

TEST_CASE("ObjModelLoader rejects a model without geometry", "[obj]")
{
	const std::string contents = "# empty model\n";

	ObjModelLoader loader;
	CHECK_THROWS(loadFromString(contents, loader));
}

TEST_CASE("ObjModelLoader ignores a quad face", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"v 1 1 0\n"
		"f 1 2 3 4\n";

	ObjModelLoader loader;
	// The only face is a quad, so no triangle geometry is produced.
	CHECK_THROWS(loadFromString(contents, loader));
	CHECK(loader.getVertices().empty());
}

TEST_CASE("ObjModelLoader keeps triangles and skips quads", "[obj]")
{
	const std::string contents =
		"v 0 0 0\n"
		"v 1 0 0\n"
		"v 0 1 0\n"
		"v 1 1 0\n"
		"f 1 2 3 4\n"
		"f 1 2 3\n";

	ObjModelLoader loader;
	REQUIRE_NOTHROW(loadFromString(contents, loader));
	CHECK(loader.getVertices().size() == 9);
}

TEST_CASE("ObjModelLoader reload replaces the previous data", "[obj]")
{
	const std::string triangle =
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
	REQUIRE_NOTHROW(loadFromString(triangle, loader));
	REQUIRE(loader.getVertices().size() == 9);

	REQUIRE_NOTHROW(loadFromString(triangle, loader));
	CHECK(loader.getVertices().size() == 9);
	CHECK(loader.getUV().size() == 6);
	CHECK(loader.getNormals().size() == 9);
}
