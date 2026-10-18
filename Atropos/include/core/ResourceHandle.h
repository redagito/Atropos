#pragma once

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

/**
* \brief Type erased view of a resource slot state.
* Used to express dependencies between resources of different types.
*/
class ResourceSlotBase
{
public:
	virtual ~ResourceSlotBase() = default;

	virtual bool isReady() const = 0;
	virtual bool isFailed() const = 0;
	virtual std::string getError() const = 0;
};

/**
* \brief Thread safe state of an asynchronously produced resource.
*
* Workers may set an error, the main thread publishes the finished resource.
* The slot only borrows the resource pointer, ownership stays with the loader.
*/
template <typename T>
class ResourceSlot : public ResourceSlotBase
{
public:
	enum class State
	{
		Pending,
		Ready, /**< Finished, value is available. */
		Failed /**< Loading failed, error holds the reason. */
	};

	ResourceSlot()
		:
		d_state(State::Pending),
		d_value(nullptr)
	{
	}

	void setValue(T* value)
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_value = value;
		d_state = State::Ready;
	}

	void setError(std::string error)
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_error = std::move(error);
		d_state = State::Failed;
	}

	State getState() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_state;
	}

	bool isReady() const override
	{
		return getState() == State::Ready;
	}

	bool isFailed() const override
	{
		return getState() == State::Failed;
	}

	T* tryGet() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_state == State::Ready ? d_value : nullptr;
	}

	/** \brief Throws std::runtime_error if failed, std::logic_error if not ready yet. */
	T* get() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		if (d_state == State::Failed)
		{
			throw std::runtime_error(d_error);
		}
		if (d_state != State::Ready)
		{
			throw std::logic_error("Resource is not ready.");
		}
		return d_value;
	}

	std::string getError() const override
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_error;
	}

private:
	mutable std::mutex d_mutex; /**< Guards all state. */
	State d_state;
	T* d_value; /**< Borrowed resource pointer. */
	std::string d_error; /**< Failure reason. */
};

/**
* \brief Non owning handle to an asynchronously loaded resource.
* Multiple handles may share the same slot.
*/
template <typename T>
class ResourceHandle
{
public:
	ResourceHandle() = default;

	explicit ResourceHandle(std::shared_ptr<ResourceSlot<T>> slot)
		:
		d_slot(std::move(slot))
	{
	}

	bool isReady() const
	{
		return d_slot != nullptr && d_slot->isReady();
	}

	bool isFailed() const
	{
		return d_slot != nullptr && d_slot->isFailed();
	}

	T* tryGet() const
	{
		return d_slot != nullptr ? d_slot->tryGet() : nullptr;
	}

	/** \brief Throws std::runtime_error if failed, std::logic_error if not ready yet. */
	T* get() const
	{
		if (d_slot == nullptr)
		{
			throw std::logic_error("Resource handle is empty.");
		}
		return d_slot->get();
	}

	const std::shared_ptr<ResourceSlot<T>>& getSlot() const
	{
		return d_slot;
	}

private:
	std::shared_ptr<ResourceSlot<T>> d_slot; /**< Shared state with the loader. */
};
