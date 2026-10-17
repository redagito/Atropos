#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
* \brief Fixed size pool of worker threads executing queued tasks.
*
* Used to run CPU only work (file parsing, decoding) off the main thread.
* Tasks must not touch OpenGL or OpenAL. Unhandled exceptions thrown by a task
* are logged and do not terminate the pool.
*/
class ThreadPool
{
public:
	/**
	* \brief Creates the pool with the requested number of workers.
	* At least one worker is always started.
	*/
	explicit ThreadPool(unsigned int workerCount);

	/**
	* \brief Signals all workers to stop and joins them.
	* Tasks already queued are still executed before the workers exit.
	*/
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	/**
	* \brief Queues a task for execution on a worker thread.
	* Tasks submitted after shutdown are ignored.
	*/
	void submit(std::function<void()> task);

	/**
	* \brief Returns the number of worker threads.
	*/
	unsigned int getWorkerCount() const;

private:
	void workerLoop();

	std::vector<std::thread> d_workers; /**< Worker threads. */
	std::mutex d_mutex; /**< Guards the task queue and stop flag. */
	std::condition_variable d_condition; /**< Signals queued work and shutdown. */
	std::queue<std::function<void()>> d_tasks; /**< Pending tasks. */
	bool d_stopping; /**< Set when the pool is shutting down. */
};
