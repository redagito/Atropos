#pragma once

class ConfigFile; /**< Config file forward declare. */

#include "game/IGameState.h"

#include "core/TCreatorMixin.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

class GameSystem : public TCreatorMixin<GameSystem>
{
public:
	GameSystem();

	~GameSystem();

	bool update(float dtime);
	
	void init(const ConfigFile& config);

	void addState(const std::string& name, IGameState* state);

	void removeState(const std::string& name);

	bool collisionsEnabled() const;

private:
	std::unordered_map<std::string, std::unique_ptr<IGameState>> d_gameStates; /**< Game states stored by name. */
	IGameState* d_activeState; /**< Current game state. */
};
