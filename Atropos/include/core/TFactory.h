#pragma once

#include <unordered_map>
#include <functional>

/**
 * \brief Generic factory class.
 */
template <typename Type, typename Key>
class TFactory
{
public:
	typedef std::function<Type*(void)> CreatorFunction; /**< Creator function type. */

	/**
	 * \brief Adds creator function.
	 */
	void addCreator(const Key& key, const CreatorFunction& creator)
	{
		d_creators[key] = creator;
	}

	/**
	 * \brief Removes creator function.
	 */
	void removeCreator(const Key& key)
	{
		d_creators.remove(key);
	}

	/**
	 * \brief Returns existence of creator function.
	 */
	bool hasCreator(const Key& key)
	{
		return d_creators.count(key) != 0;
	}

	/**
	 * \brief Creates specified type.
	 */
	Type* create(const Key& key)
	{
		return d_creators.at(key)();
	}

private:
	std::unordered_map<Key, CreatorFunction> d_creators; /**< Creator function map. */
};
