#include "game/GameSystem.h"

#include "io/ConfigFile.h"
#include <spdlog/spdlog.h>



GameSystem::~GameSystem()
{
	SPDLOG_DEBUG("Game system destroyed.");
}

GameSystem::GameSystem()
	:
	d_activeState(nullptr)
{
	SPDLOG_DEBUG("Game system created.");
}

// Updates game objects
bool GameSystem::update(float dtime)
{
	// Exit if state not set
	if (d_activeState == nullptr)
	{
		return false;
	}

	// Update current state
	if (!d_activeState->update(dtime))
	{
		// State ended
		d_activeState->onExit();
		// Check state exists
		if (d_gameStates.count(d_activeState->getNextState()) == 0)
		{
			// State transition error
			return false;
		}
		// Set next state
		d_activeState = d_gameStates.at(d_activeState->getNextState()).get();
		d_activeState->onEnter();
	}
	// Update success
	return true;
}

bool GameSystem::init(const ConfigFile& config)
{
	SPDLOG_DEBUG("Initializing game system.");

	// Get initial state property
	std::string initialState = config.get("game_initialstate", "load");
	SPDLOG_DEBUG("Game system initial state set to {}.", initialState.c_str());

	// Set initial state active
	const auto it = d_gameStates.find(initialState);
	if (it == d_gameStates.end())
	{
		SPDLOG_ERROR("Initial game state {} is not registered.", initialState.c_str());
		return false;
	}
	d_activeState = it->second.get();

	// On enter state trigger
	d_activeState->onEnter();
	return true;
}

bool GameSystem::addState(const std::string& name, IGameState* state)
{
	if (!state->init())
	{
		SPDLOG_ERROR("Failed to initialize game state {}.", name.c_str());
		delete state;
		return false;
	}
	const auto existing = d_gameStates.find(name);
	const bool wasActive = (existing != d_gameStates.end() && d_activeState == existing->second.get());
	if (existing != d_gameStates.end())
	{
		// State with name already exists
		SPDLOG_WARN("Overwriting game state {}.", name.c_str());
		if (wasActive)
		{
			existing->second->onExit();
		}
	}

	d_gameStates[name] = std::unique_ptr<IGameState>(state);
	if (wasActive)
	{
		d_activeState = state;
		d_activeState->onEnter();
	}
	SPDLOG_DEBUG("Added game state {} to game system.", name.c_str());
	return true;
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
