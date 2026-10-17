#pragma once

#include "core/ResourceHandle.h"
#include "core/ThreadPool.h"

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
* \brief Loads resources of a single type asynchronously.
*
* Parsing runs on the thread pool and must not touch OpenGL or OpenAL. The
* finished resource is created on the main thread by the committer during
* process(). A resource is only committed once all of its dependencies are
* ready. If a dependency fails, the resource fails with the same reason.
*
* \tparam T Finished resource type.
* \tparam Parsed CPU side data produced by the parser and consumed by the committer.
*/
template <typename T, typename Parsed>
class AsyncLoader
{
public:
	using Parser = std::function<Parsed()>;
	using Committer = std::function<std::unique_ptr<T>(const Parsed&)>;
	using DependencyFn = std::function<std::vector<std::shared_ptr<ResourceSlotBase>>(const Parsed&)>;

	/**
	* \brief Creates a loader using the given pool for parsing.
	*/
	explicit AsyncLoader(ThreadPool& pool)
		:
		d_pool(pool)
	{
	}

	/**
	* \brief Requests a resource by key, scheduling a parse on a worker.
	* Repeated requests for the same key share one load.
	* \param deps Dependencies known up front.
	* \param depsFn Resolves dependencies that are only known after parsing,
	* runs on the worker thread.
	*/
	ResourceHandle<T> request(const std::string& key, Parser parser, Committer committer,
		std::vector<std::shared_ptr<ResourceSlotBase>> deps = {}, DependencyFn depsFn = {})
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		const auto it = d_entries.find(key);
		if (it != d_entries.end())
		{
			return ResourceHandle<T>(it->second->slot);
		}

		auto entry = std::make_unique<Entry>();
		entry->slot = std::make_shared<ResourceSlot<T>>();
		entry->committer = std::move(committer);
		entry->deps = std::move(deps);
		ResourceHandle<T> handle(entry->slot);

		d_entries.emplace(key, std::move(entry));

		d_pool.submit([this, key, parser, depsFn]()
		{
			try
			{
				std::unique_ptr<Parsed> parsed = std::make_unique<Parsed>(parser());
				std::vector<std::shared_ptr<ResourceSlotBase>> extraDeps;
				if (depsFn)
				{
					extraDeps = depsFn(*parsed);
				}
				storeParsed(key, std::move(parsed), std::move(extraDeps));
			}
			catch (const std::exception& e)
			{
				storeParseError(key, e.what());
			}
		});

		return handle;
	}

	/**
	* \brief True if a resource with the given key has been requested.
	*/
	bool contains(const std::string& key) const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_entries.count(key) != 0;
	}

	/**
	* \brief Returns the handle of an already requested resource, empty otherwise.
	*/
	ResourceHandle<T> getHandle(const std::string& key) const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		const auto it = d_entries.find(key);
		if (it == d_entries.end())
		{
			return ResourceHandle<T>();
		}
		return ResourceHandle<T>(it->second->slot);
	}

	/**
	* \brief Commits all ready resources. Must run on the main thread.
	* Repeatedly scans until no further resource can be committed, so a whole
	* dependency chain finishes in one call.
	*
	* \warning The committer runs while this loader's lock is held, so it must
	* not call back into the loader (request, getHandle, process, ...) or it
	* will deadlock.
	*/
	void process()
	{
		std::lock_guard<std::mutex> lock(d_mutex);

		bool progress = true;
		while (progress)
		{
			progress = false;
			for (auto& pair : d_entries)
			{
				Entry& entry = *pair.second;
				if (entry.committed || entry.slot->isFailed() || !entry.parseDone)
				{
					continue;
				}

				if (failOnFailedDependency(entry))
				{
					continue;
				}
				if (!dependenciesReady(entry))
				{
					continue;
				}

				try
				{
					entry.resource = entry.committer(*entry.parsed);
					entry.slot->setValue(entry.resource.get());
					entry.committed = true;
					// The parsed data is only needed until the committer has
					// run. Keeping it would retain a full copy of every parsed
					// buffer, such as the decoded PCM of a soundtrack.
					entry.parsed.reset();
					progress = true;
				}
				catch (const std::exception& e)
				{
					entry.slot->setError(e.what());
				}
			}
		}
	}

	/**
	* \brief True when all requested resources are ready or failed.
	*/
	bool isIdle() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		for (const auto& pair : d_entries)
		{
			if (!pair.second->slot->isReady() && !pair.second->slot->isFailed())
			{
				return false;
			}
		}
		return true;
	}

	/**
	* \brief True if any requested resource failed.
	*/
	bool hasError() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		for (const auto& pair : d_entries)
		{
			if (pair.second->slot->isFailed())
			{
				return true;
			}
		}
		return false;
	}

private:
	struct Entry
	{
		std::shared_ptr<ResourceSlot<T>> slot; /**< Shared state with the handle. */
		Committer committer; /**< Creates the resource on the main thread. */
		std::vector<std::shared_ptr<ResourceSlotBase>> deps; /**< Must be ready before commit. */
		std::unique_ptr<Parsed> parsed; /**< Parsed data from the worker. */
		std::unique_ptr<T> resource; /**< Committed resource. */
		bool parseDone = false; /**< Parsing finished successfully. */
		bool committed = false; /**< Resource committed. */
	};

	static bool dependenciesReady(const Entry& entry)
	{
		for (const auto& dep : entry.deps)
		{
			if (!dep->isReady())
			{
				return false;
			}
		}
		return true;
	}

	static bool failOnFailedDependency(Entry& entry)
	{
		for (const auto& dep : entry.deps)
		{
			if (dep->isFailed())
			{
				entry.slot->setError(dep->getError());
				return true;
			}
		}
		return false;
	}

	void storeParsed(const std::string& key, std::unique_ptr<Parsed> parsed,
		std::vector<std::shared_ptr<ResourceSlotBase>> extraDeps)
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		const auto it = d_entries.find(key);
		if (it == d_entries.end())
		{
			return;
		}
		it->second->parsed = std::move(parsed);
		it->second->parseDone = true;
		for (auto& dep : extraDeps)
		{
			it->second->deps.push_back(std::move(dep));
		}
	}

	void storeParseError(const std::string& key, const std::string& error)
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		const auto it = d_entries.find(key);
		if (it == d_entries.end())
		{
			return;
		}
		it->second->slot->setError(error);
	}

	mutable std::mutex d_mutex; /**< Guards entries and state. */
	ThreadPool& d_pool; /**< Pool used to run parsers. */
	std::unordered_map<std::string, std::unique_ptr<Entry>> d_entries; /**< Requested resources. */
};
