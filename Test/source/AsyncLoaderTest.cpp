#include "core/AsyncLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{
	void settle(AsyncLoader<int, int>& loader)
	{
		for (int i = 0; i < 5000; ++i)
		{
			loader.process();
			if (loader.isIdle())
			{
				return;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

TEST_CASE("AsyncLoader produces a value asynchronously", "[asyncloader]")
{
	ThreadPool pool(1);
	AsyncLoader<int, int> loader(pool);

	ResourceHandle<int> handle = loader.request("value",
		[] { return 5; },
		[](const int& parsed) { return std::make_unique<int>(parsed); });

	CHECK_FALSE(handle.isReady());
	settle(loader);

	REQUIRE(handle.isReady());
	CHECK(*handle.get() == 5);
}

TEST_CASE("AsyncLoader deduplicates requests by key", "[asyncloader]")
{
	ThreadPool pool(2);
	AsyncLoader<int, int> loader(pool);

	std::atomic<int> parseCount{ 0 };
	auto parser = [&parseCount] { ++parseCount; return 1; };
	auto committer = [](const int& value) { return std::make_unique<int>(value); };

	ResourceHandle<int> first = loader.request("same", parser, committer);
	ResourceHandle<int> second = loader.request("same", parser, committer);

	settle(loader);

	CHECK(parseCount.load() == 1);
	CHECK(first.get() == second.get());
}

TEST_CASE("AsyncLoader reports a parser error", "[asyncloader]")
{
	ThreadPool pool(1);
	AsyncLoader<int, int> loader(pool);

	ResourceHandle<int> handle = loader.request("bad",
		[]() -> int { throw std::runtime_error("parse failed"); },
		[](const int& value) { return std::make_unique<int>(value); });

	settle(loader);

	REQUIRE(handle.isFailed());

	bool threw = false;
	try
	{
		handle.get();
	}
	catch (const std::runtime_error& e)
	{
		threw = true;
		CHECK(std::string(e.what()) == "parse failed");
	}
	CHECK(threw);
}

TEST_CASE("AsyncLoader commits dependencies before dependents", "[asyncloader]")
{
	ThreadPool pool(2);
	AsyncLoader<int, int> loader(pool);

	std::vector<std::string> commitOrder;

	ResourceHandle<int> dep = loader.request("dep",
		[] { return 1; },
		[&commitOrder](const int& value)
		{
			commitOrder.push_back("dep");
			return std::make_unique<int>(value);
		});

	ResourceHandle<int> dependent = loader.request("dependent",
		[] { return 2; },
		[&commitOrder](const int& value)
		{
			commitOrder.push_back("dependent");
			return std::make_unique<int>(value);
		},
		{ dep.getSlot() });

	settle(loader);

	REQUIRE(dep.isReady());
	REQUIRE(dependent.isReady());
	REQUIRE(commitOrder.size() == 2);
	CHECK(commitOrder[0] == "dep");
	CHECK(commitOrder[1] == "dependent");
}

TEST_CASE("AsyncLoader fails dependents of a failed resource", "[asyncloader]")
{
	ThreadPool pool(2);
	AsyncLoader<int, int> loader(pool);

	auto committer = [](const int& value) { return std::make_unique<int>(value); };

	ResourceHandle<int> dep = loader.request("dep",
		[]() -> int { throw std::runtime_error("boom"); },
		committer);

	ResourceHandle<int> dependent = loader.request("dependent",
		[] { return 2; },
		committer,
		{ dep.getSlot() });

	settle(loader);

	CHECK(dep.isFailed());
	CHECK(dependent.isFailed());
}

TEST_CASE("AsyncLoader resolves dependencies discovered during parsing", "[asyncloader]")
{
	ThreadPool pool(2);
	AsyncLoader<int, int> loader(pool);

	std::vector<std::string> commitOrder;

	ResourceHandle<int> dep = loader.request("dep",
		[] { return 1; },
		[&commitOrder](const int& value)
		{
			commitOrder.push_back("dep");
			return std::make_unique<int>(value);
		});

	ResourceHandle<int> dependent = loader.request("dependent",
		[] { return 2; },
		[&commitOrder](const int& value)
		{
			commitOrder.push_back("dependent");
			return std::make_unique<int>(value);
		},
		{},
		[&dep](const int&) { return std::vector<std::shared_ptr<ResourceSlotBase>>{ dep.getSlot() }; });

	settle(loader);

	REQUIRE(dependent.isReady());
	REQUIRE(commitOrder.size() == 2);
	CHECK(commitOrder[0] == "dep");
	CHECK(commitOrder[1] == "dependent");
}

TEST_CASE("AsyncLoader commits each resource once", "[asyncloader]")
{
	ThreadPool pool(1);
	AsyncLoader<int, int> loader(pool);

	std::atomic<int> commits{ 0 };

	ResourceHandle<int> handle = loader.request("once",
		[] { return 1; },
		[&commits](const int& value)
		{
			++commits;
			return std::make_unique<int>(value);
		});

	settle(loader);
	loader.process();
	loader.process();

	CHECK(commits.load() == 1);
	CHECK(loader.isIdle());
}
