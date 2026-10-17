#include "game/GameSystem.h"

#include "io/ConfigFile.h"
#include "debug/Log.h"

// Custom memory manager


GameSystem::~GameSystem()
{
	LOG_DEBUG("Game system destroyed.");
}

GameSystem::GameSystem()
	:
	d_activeState(nullptr)
{
	LOG_DEBUG("Game system created.");
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
	LOG_DEBUG("Initializing game system.");

	// Get initial state property
	std::string initialState = config.get("game_initialstate", "load");
	LOG_DEBUG("Game system initial state set to %s.", initialState.c_str());

	// Set initial state active
	const auto it = d_gameStates.find(initialState);
	if (it == d_gameStates.end())
	{
		LOG_ERROR("Initial game state %s is not registered.", initialState.c_str());
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
		LOG_ERROR("Failed to initialize game state %s.", name.c_str());
		delete state;
		return false;
	}
	if (d_gameStates.count(name) != 0)
	{
		// State with name already exists
		LOG_WARNING("Overwriting game state %s.", name.c_str());
	}

	d_gameStates[name] = std::unique_ptr<IGameState>(state);
	LOG_DEBUG("Added game state %s to game system.", name.c_str());
	return true;
}

void GameSystem::removeState(const std::string& name)
{
	if (d_gameStates.count(name) == 0)
	{
		LOG_WARNING("Could not remove state %s, the state does not exist.", name.c_str());
		return;
	}
	d_gameStates.erase(name);
	LOG_DEBUG("Removed game state %s from game system.", name.c_str());
}
