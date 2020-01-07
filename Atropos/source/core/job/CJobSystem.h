#pragma once

#include "util/TThreadSafeQueue.h"
#include "core/TSingletonMixin.h"

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>

class CJobSystem : public TSingletonMixin<CJobSystem>
{
public:
	typedef std::function<void(void)> JobFunction;

	/**
	* \brief Starts job system.
	* Creates thread pool.
	*/
	CJobSystem();

	/**
	* \brief Finishes pending tasks.
	*/
	~CJobSystem();

	/**
	* \brief Stops pending job execution.
	*/
	void stop();

	/**
	* \brief Starts job execution.
	*/
	void start();

	/**
	* \brief Adds job to execution queue.
	*/
	void addJob(const JobFunction& job);

	/**
	* \brief Blocks until all jobs are finished.
	*/
	void finishJobs();

private:
	/**
	* \brief Work function executed by threads.
	*/
	void work();

	std::vector<std::thread> d_threads; /**< Thread pool. */
	TThreadSafeQueue<JobFunction> d_jobQueue; /**< Stores pending jobs. */

	unsigned int d_threadCount; /**< Number of threads used. */

	std::atomic<bool> d_done; /**< Flag for stopping worker threads. */
	std::atomic<bool> d_stop; /**< Execution started/stopped flag. */
};
