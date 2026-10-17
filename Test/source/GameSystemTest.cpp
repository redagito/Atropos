#include "game/GameSystem.h"

#include "game/IGameState.h"
#include "io/ConfigFile.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

namespace
{
	struct StateLog
	{
		int enters = 0;
		int exits = 0;
		bool updateResult = true;
		std::string nextState;
	};

	class FakeState : public IGameState
	{
	public:
		explicit FakeState(StateLog* log)
			:
			d_log(log)
		{
		}

		bool init() override
		{
			return true;
		}

		void onEnter() override
		{
			++d_log->enters;
		}

		bool update(float) override
		{
			return d_log->updateResult;
		}

		void onExit() override
		{
			++d_log->exits;
		}

		const std::string& getNextState() const override
		{
			return d_log->nextState;
		}

	private:
		StateLog* d_log;
	};
}

TEST_CASE("GameSystem replacing the active state exits the old state", "[gamesystem]")
{
	GameSystem system;
	StateLog firstLog;
	StateLog secondLog;

	REQUIRE(system.addState("play", new FakeState(&firstLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE(system.init(config));
	REQUIRE(firstLog.enters == 1);

	REQUIRE(system.addState("play", new FakeState(&secondLog)));

	CHECK(firstLog.exits == 1);
	CHECK(secondLog.enters == 1);
}

TEST_CASE("GameSystem removing the active state leaves no active state", "[gamesystem]")
{
	GameSystem system;
	StateLog log;
	REQUIRE(system.addState("play", new FakeState(&log)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE(system.init(config));

	system.removeState("play");

	CHECK_FALSE(system.update(0.f));
}

TEST_CASE("GameSystem removing another state keeps the active state", "[gamesystem]")
{
	GameSystem system;
	StateLog activeLog;
	StateLog otherLog;
	REQUIRE(system.addState("play", new FakeState(&activeLog)));
	REQUIRE(system.addState("pause", new FakeState(&otherLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE(system.init(config));

	system.removeState("pause");

	CHECK(system.update(0.f));
}
