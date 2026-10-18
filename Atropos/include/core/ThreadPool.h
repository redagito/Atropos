#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
* \brief Fixed size pool of worker threads executing queued tasks.
* Tasks must not touch OpenGL or OpenAL; thrown exceptions are logged, not fatal.
*/
class ThreadPool
{
public:
	/** \brief At least one worker is always started. */
	explicit ThreadPool(unsigned int workerCount);

	/** \brief Stops and joins all workers; tasks queued before shutdown still run. */
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	/** \brief Queues a task; submissions after shutdown are silently ignored. */
	void submit(std::function<void()> task);

	unsigned int getWorkerCount() const;

private:
	void workerLoop();

	std::vector<std::thread> d_workers;
	std::mutex d_mutex; /**< Guards the task queue and stop flag. */
	std::condition_variable d_condition; /**< Signals queued work and shutdown. */
	std::queue<std::function<void()>> d_tasks;
	bool d_stopping;
};
