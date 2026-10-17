#include "io/CConfigFile.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

#include <string>

TEST_CASE("CConfigFile stores and returns string values", "[config]")
{
	CConfigFile config;
	config.set("key", "value");

	REQUIRE(config.has("key"));
	REQUIRE(config.get("key", std::string("default")) == "value");
}

TEST_CASE("CConfigFile returns the default for unknown keys", "[config]")
{
	CConfigFile config;

	REQUIRE_FALSE(config.has("missing"));
	REQUIRE(config.get("missing", std::string("default")) == "default");
}

TEST_CASE("CConfigFile converts typed values", "[config]")
{
	CConfigFile config;
	config.set("count", "42");
	config.set("scale", "1.5");
	config.set("origin", "1 2 3");

	REQUIRE(config.get("count", 0) == 42);
	REQUIRE(config.get("scale", 0.f) == 1.5f);
	REQUIRE(config.get("origin", glm::vec3(0.f)) == glm::vec3(1.f, 2.f, 3.f));
}

TEST_CASE("CConfigFile falls back to defaults for malformed values", "[config]")
{
	CConfigFile config;
	config.set("count", "not a number");

	REQUIRE(config.get("count", 7) == 7);
}
