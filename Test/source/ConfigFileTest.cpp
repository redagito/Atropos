#include "io/ConfigFile.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

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

TEST_CASE("ConfigFile returns the default vec3 for a malformed value", "[config]")
{
	ConfigFile config;
	config.set("origin", "not a vec3");

	CHECK(config.get("origin", glm::vec3(9.f)) == glm::vec3(9.f));
}

TEST_CASE("ConfigFile load replaces previously loaded values", "[config]")
{
	const std::filesystem::path first = std::filesystem::temp_directory_path() / "atropos_config_first.cfg";
	const std::filesystem::path second = std::filesystem::temp_directory_path() / "atropos_config_second.cfg";
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
