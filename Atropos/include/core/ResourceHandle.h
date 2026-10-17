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
		Pending, /**< Not finished yet. */
		Ready, /**< Finished, value is available. */
		Failed /**< Loading failed, error holds the reason. */
	};

	ResourceSlot()
		:
		d_state(State::Pending),
		d_value(nullptr)
	{
	}

	/**
	* \brief Publishes the finished resource.
	*/
	void setValue(T* value)
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_value = value;
		d_state = State::Ready;
	}

	/**
	* \brief Marks the resource as failed.
	*/
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

	/**
	* \brief Returns the resource or nullptr if it is not ready.
	*/
	T* tryGet() const
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_state == State::Ready ? d_value : nullptr;
	}

	std::string getError() const override
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_error;
	}

private:
	mutable std::mutex d_mutex; /**< Guards all state. */
	State d_state; /**< Current loading state. */
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

	/**
	* \brief Returns the resource or nullptr if it is not ready.
	*/
	T* tryGet() const
	{
		return d_slot != nullptr ? d_slot->tryGet() : nullptr;
	}

	/**
	* \brief Returns the resource.
	* Throws std::runtime_error if loading failed and std::logic_error if the
	* resource is not ready yet.
	*/
	T* get() const
	{
		if (d_slot == nullptr)
		{
			throw std::logic_error("Resource handle is empty.");
		}
		if (d_slot->isFailed())
		{
			throw std::runtime_error(d_slot->getError());
		}
		T* value = d_slot->tryGet();
		if (value == nullptr)
		{
			throw std::logic_error("Resource is not ready.");
		}
		return value;
	}

	/**
	* \brief Returns the shared slot, may be null.
	*/
	const std::shared_ptr<ResourceSlot<T>>& getSlot() const
	{
		return d_slot;
	}

private:
	std::shared_ptr<ResourceSlot<T>> d_slot; /**< Shared state with the loader. */
};
