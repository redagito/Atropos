#include "core/ThreadPool.h"

#include <spdlog/spdlog.h>

#include <exception>
#include <utility>

ThreadPool::ThreadPool(unsigned int workerCount)
	:
	d_stopping(false)
{
	if (workerCount < 1)
	{
		workerCount = 1;
	}

	d_workers.reserve(workerCount);
	for (unsigned int i = 0; i < workerCount; ++i)
	{
		d_workers.emplace_back([this] { workerLoop(); });
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_stopping = true;
	}
	d_condition.notify_all();

	for (std::thread& worker : d_workers)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}

void ThreadPool::submit(std::function<void()> task)
{
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		if (d_stopping)
		{
			return;
		}
		d_tasks.push(std::move(task));
	}
	d_condition.notify_one();
}

unsigned int ThreadPool::getWorkerCount() const
{
	return static_cast<unsigned int>(d_workers.size());
}

void ThreadPool::workerLoop()
{
	for (;;)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(d_mutex);
			d_condition.wait(lock, [this] { return d_stopping || !d_tasks.empty(); });
			if (d_stopping && d_tasks.empty())
			{
				return;
			}
			task = std::move(d_tasks.front());
			d_tasks.pop();
		}

		try
		{
			task();
		}
		catch (const std::exception& e)
		{
			SPDLOG_ERROR("Unhandled exception in a worker task: {}", e.what());
		}
		catch (...)
		{
			SPDLOG_ERROR("Unhandled non-standard exception in a worker task.");
		}
	}
}
