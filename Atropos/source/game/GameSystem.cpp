#include "game/GameSystem.h"

#include "io/ConfigFile.h"

#include <spdlog/spdlog.h>

#include <stdexcept>



GameSystem::~GameSystem()
{
	// onExit must run before the state is destroyed, as in a normal transition.
	if (d_activeState != nullptr)
	{
		d_activeState->onExit();
		d_activeState = nullptr;
	}
	SPDLOG_DEBUG("Game system destroyed.");
}

GameSystem::GameSystem()
	:
	d_activeState(nullptr)
{
	SPDLOG_DEBUG("Game system created.");
}

bool GameSystem::update(float dtime)
{
	if (d_activeState == nullptr)
	{
		return false;
	}

	if (!d_activeState->update(dtime))
	{
		d_activeState->onExit();

		const std::string& nextState = d_activeState->getNextState();
		if (nextState == QUIT_STATE_NAME)
		{
			d_activeState = nullptr;
			return false;
		}

		const auto it = d_gameStates.find(nextState);
		if (it == d_gameStates.end())
		{
			throw std::runtime_error("Game state transition to an unregistered state: " + nextState);
		}

		d_activeState = it->second.get();
		d_activeState->onEnter();
	}
	return true;
}

void GameSystem::init(const ConfigFile& config)
{
	SPDLOG_DEBUG("Initializing game system.");

	const std::string initialState = config.get("game_initialstate", "load");
	SPDLOG_DEBUG("Game system initial state set to {}.", initialState);

	const auto it = d_gameStates.find(initialState);
	if (it == d_gameStates.end())
	{
		throw std::runtime_error("Initial game state is not registered: " + initialState);
	}
	d_activeState = it->second.get();

	d_activeState->onEnter();
}

void GameSystem::addState(const std::string& name, IGameState* state)
{
	// Take ownership before init so a throwing init does not leak.
	std::unique_ptr<IGameState> owned(state);
	owned->init();

	const auto existing = d_gameStates.find(name);
	const bool wasActive = (existing != d_gameStates.end() && d_activeState == existing->second.get());
	if (existing != d_gameStates.end())
	{
		SPDLOG_WARN("Overwriting game state {}.", name.c_str());
		if (wasActive)
		{
			existing->second->onExit();
		}
	}

	d_gameStates[name] = std::move(owned);
	if (wasActive)
	{
		d_activeState = d_gameStates[name].get();
		d_activeState->onEnter();
	}
	SPDLOG_DEBUG("Added game state {} to game system.", name);
}

bool GameSystem::collisionsEnabled() const
{
	if (d_activeState == nullptr)
	{
		return false;
	}
	return d_activeState->collisionsEnabled();
}

void GameSystem::removeState(const std::string& name)
{
	const auto it = d_gameStates.find(name);
	if (it == d_gameStates.end())
	{
		SPDLOG_WARN("Could not remove state {}, the state does not exist.", name.c_str());
		return;
	}
	if (d_activeState == it->second.get())
	{
		d_activeState = nullptr;
	}
	d_gameStates.erase(it);
	SPDLOG_DEBUG("Removed game state {} from game system.", name.c_str());
}
