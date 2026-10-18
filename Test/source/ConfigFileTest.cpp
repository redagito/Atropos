#include "io/ConfigFile.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

#include <process.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace
{
	void writeFile(const std::filesystem::path& path, const std::string& contents)
	{
		std::ofstream out(path);
		out << contents;
	}
}

TEST_CASE("ConfigFile stores and returns string values", "[config]")
{
	ConfigFile config;
	config.set("key", "value");

	REQUIRE(config.has("key"));
	REQUIRE(config.get("key", std::string("default")) == "value");
}

TEST_CASE("ConfigFile returns the default for unknown keys", "[config]")
{
	ConfigFile config;

	REQUIRE_FALSE(config.has("missing"));
	REQUIRE(config.get("missing", std::string("default")) == "default");
}

TEST_CASE("ConfigFile converts typed values", "[config]")
{
	ConfigFile config;
	config.set("count", "42");
	config.set("scale", "1.5");
	config.set("origin", "1 2 3");

	REQUIRE(config.get("count", 0) == 42);
	REQUIRE(config.get("scale", 0.f) == 1.5f);
	REQUIRE(config.get("origin", glm::vec3(0.f)) == glm::vec3(1.f, 2.f, 3.f));
}

TEST_CASE("ConfigFile zero-fills missing vec3 components", "[config]")
{
	ConfigFile config;
	config.set("origin", "0.5");

	CHECK(config.get("origin", glm::vec3(9.f)) == glm::vec3(0.5f, 0.f, 0.f));
}

TEST_CASE("ConfigFile falls back to defaults for malformed values", "[config]")
{
	ConfigFile config;
	config.set("count", "not a number");

	REQUIRE(config.get("count", 7) == 7);
}

TEST_CASE("ConfigFile rejects a number with trailing characters", "[config]")
{
	ConfigFile config;
	config.set("count", "800x");
	config.set("scale", "0.5 meters");

	// A numeric prefix would otherwise be read as a valid value.
	REQUIRE(config.get("count", 7) == 7);
	REQUIRE(config.get("scale", 1.f) == 1.f);
}

TEST_CASE("ConfigFile returns the default vec3 for a malformed value", "[config]")
{
	ConfigFile config;
	config.set("origin", "not a vec3");

	CHECK(config.get("origin", glm::vec3(9.f)) == glm::vec3(9.f));
}

TEST_CASE("ConfigFile skips a whitespace-only line", "[config]")
{
	// Process id keeps the temp file name unique across concurrent ctest runs.
	const std::string id = std::to_string(_getpid());
	const std::filesystem::path path = std::filesystem::temp_directory_path() / ("atropos_config_" + id + "_whitespace.cfg");
	writeFile(path, "alpha=1\n   \nbeta=2\n");

	ConfigFile config;
	REQUIRE_NOTHROW(config.load(path.string()));
	std::filesystem::remove(path);
	REQUIRE(config.get("alpha", std::string()) == "1");
	REQUIRE(config.get("beta", std::string()) == "2");
}

TEST_CASE("ConfigFile skips an indented comment", "[config]")
{
	const std::string id = std::to_string(_getpid());
	const std::filesystem::path path = std::filesystem::temp_directory_path() / ("atropos_config_" + id + "_indented.cfg");
	writeFile(path, "alpha=1\n  # note\nbeta=2\n");

	ConfigFile config;
	REQUIRE_NOTHROW(config.load(path.string()));
	std::filesystem::remove(path);
	REQUIRE(config.get("alpha", std::string()) == "1");
	REQUIRE(config.get("beta", std::string()) == "2");
}

TEST_CASE("ConfigFile skips a carriage return only line", "[config]")
{
	const std::string id = std::to_string(_getpid());
	const std::filesystem::path path = std::filesystem::temp_directory_path() / ("atropos_config_" + id + "_carriage.cfg");
	writeFile(path, "alpha=1\r\n\r\nbeta=2\r\n");

	ConfigFile config;
	REQUIRE_NOTHROW(config.load(path.string()));
	std::filesystem::remove(path);
	REQUIRE(config.get("alpha", std::string()) == "1");
	REQUIRE(config.get("beta", std::string()) == "2");
}

TEST_CASE("ConfigFile load replaces previously loaded values", "[config]")
{
	// Process id keeps the temp file names unique across concurrent ctest runs.
	const std::string id = std::to_string(_getpid());
	const std::filesystem::path first = std::filesystem::temp_directory_path() / ("atropos_config_" + id + "_first.cfg");
	const std::filesystem::path second = std::filesystem::temp_directory_path() / ("atropos_config_" + id + "_second.cfg");
	writeFile(first, "alpha=1\n");
	writeFile(second, "beta=2\n");

	ConfigFile config;
	config.load(first.string());
	REQUIRE(config.has("alpha"));

	config.load(second.string());
	CHECK_FALSE(config.has("alpha"));
	CHECK(config.has("beta"));

	std::filesystem::remove(first);
	std::filesystem::remove(second);
}
