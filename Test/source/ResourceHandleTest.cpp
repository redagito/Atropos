#include "core/ResourceHandle.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>

TEST_CASE("ResourceHandle reports a pending slot", "[resourcehandle]")
{
	auto slot = std::make_shared<ResourceSlot<int>>();
	ResourceHandle<int> handle(slot);

	CHECK_FALSE(handle.isReady());
	CHECK_FALSE(handle.isFailed());
	CHECK(handle.tryGet() == nullptr);
	CHECK_THROWS_AS(handle.get(), std::logic_error);
}

TEST_CASE("ResourceHandle returns a published value", "[resourcehandle]")
{
	int value = 42;
	auto slot = std::make_shared<ResourceSlot<int>>();
	ResourceHandle<int> handle(slot);

	slot->setValue(&value);

	REQUIRE(handle.isReady());
	CHECK(handle.tryGet() == &value);
	CHECK(handle.get() == &value);
}

TEST_CASE("ResourceHandle reports a loading failure", "[resourcehandle]")
{
	auto slot = std::make_shared<ResourceSlot<int>>();
	ResourceHandle<int> handle(slot);

	slot->setError("boom");

	REQUIRE(handle.isFailed());
	CHECK(handle.tryGet() == nullptr);

	bool threw = false;
	try
	{
		handle.get();
	}
	catch (const std::runtime_error& e)
	{
		threw = true;
		CHECK(std::string(e.what()) == "boom");
	}
	CHECK(threw);
}

TEST_CASE("ResourceHandle shared slots observe the same state", "[resourcehandle]")
{
	int value = 7;
	auto slot = std::make_shared<ResourceSlot<int>>();
	ResourceHandle<int> first(slot);
	ResourceHandle<int> second(slot);

	slot->setValue(&value);

	CHECK(first.get() == &value);
	CHECK(second.get() == &value);
}

TEST_CASE("Default constructed ResourceHandle is empty", "[resourcehandle]")
{
	ResourceHandle<int> handle;

	CHECK_FALSE(handle.isReady());
	CHECK(handle.tryGet() == nullptr);
	CHECK_THROWS_AS(handle.get(), std::logic_error);
}
