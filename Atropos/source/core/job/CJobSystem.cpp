#include "CJobSystem.h"

#include <iostream>

CJobSystem::CJobSystem()
	:
	d_done(false),
	d_stop(false),
	d_threadCount(0)
{
	// Number of threads is based on available processors.
	d_threadCount = std::thread::hardware_concurrency();
	if (d_threadCount == 0)
	{
		// If hardware stats are unavailable, force 4 threads as default.
		d_threadCount = 4;
	}
	// Create threads and queues
	for (unsigned int i = 0; i < d_threadCount; ++i)
	{
		d_threads.push_back(std::thread(&CJobSystem::work, this));
	}
}

CJobSystem::~CJobSystem()
{
	// Stop job retrieval
	d_done = true;
	// Finish current tasks
	for (auto& thread : d_threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}

void CJobSystem::addJob(const CJobSystem::JobFunction& job)
{
	d_jobQueue.push(job);
	return;
}

void CJobSystem::stop()
{
	d_stop = true;
}

void CJobSystem::start()
{
	d_stop = false;
}

void CJobSystem::finishJobs()
{
	while (!d_jobQueue.empty())
	{
		std::this_thread::yield();
	}	
}

void CJobSystem::work()
{
	while (!d_done)
	{
		JobFunction job;
		if (!d_stop && d_jobQueue.pop(job))
		{
			job();
		}
		else
		{
			std::this_thread::yield();
		}
	}
}
