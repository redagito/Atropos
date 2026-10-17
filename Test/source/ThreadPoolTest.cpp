#include "core/ThreadPool.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <latch>

TEST_CASE("ThreadPool executes all submitted tasks", "[threadpool]")
{
	ThreadPool pool(4);

	constexpr int taskCount = 100;
	std::atomic<int> counter{ 0 };
	std::latch done(taskCount);

	for (int i = 0; i < taskCount; ++i)
	{
		pool.submit([&counter, &done]
		{
			++counter;
			done.count_down();
		});
	}

	done.wait();
	CHECK(counter.load() == taskCount);
}

TEST_CASE("ThreadPool always starts at least one worker", "[threadpool]")
{
	ThreadPool none(0);
	CHECK(none.getWorkerCount() == 1);

	ThreadPool two(2);
	CHECK(two.getWorkerCount() == 2);
}

TEST_CASE("ThreadPool drains queued tasks on shutdown", "[threadpool]")
{
	std::atomic<int> counter{ 0 };
	{
		ThreadPool pool(1);
		for (int i = 0; i < 50; ++i)
		{
			pool.submit([&counter] { ++counter; });
		}
	}

	CHECK(counter.load() == 50);
}
