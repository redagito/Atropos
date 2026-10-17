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

		void init() override
		{
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

	REQUIRE_NOTHROW(system.addState("play", new FakeState(&firstLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));
	REQUIRE(firstLog.enters == 1);

	REQUIRE_NOTHROW(system.addState("play", new FakeState(&secondLog)));

	CHECK(firstLog.exits == 1);
	CHECK(secondLog.enters == 1);
}

TEST_CASE("GameSystem init throws for an unregistered initial state", "[gamesystem]")
{
	GameSystem system;

	ConfigFile config;
	config.set("game_initialstate", "missing");

	CHECK_THROWS(system.init(config));
}

TEST_CASE("GameSystem transitions to a registered next state", "[gamesystem]")
{
	GameSystem system;
	StateLog playLog;
	StateLog pauseLog;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&playLog)));
	REQUIRE_NOTHROW(system.addState("pause", new FakeState(&pauseLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	playLog.updateResult = false;
	playLog.nextState = "pause";

	CHECK(system.update(0.f));
	CHECK(playLog.exits == 1);
	CHECK(pauseLog.enters == 1);
}

TEST_CASE("GameSystem stops when a state requests the quit state", "[gamesystem]")
{
	GameSystem system;
	StateLog log;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&log)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	log.updateResult = false;
	log.nextState = QUIT_STATE_NAME;

	CHECK_FALSE(system.update(0.f));
	CHECK(log.exits == 1);
}

TEST_CASE("GameSystem throws on a transition to an unregistered state", "[gamesystem]")
{
	GameSystem system;
	StateLog log;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&log)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	log.updateResult = false;
	log.nextState = "missing";

	CHECK_THROWS(system.update(0.f));
}

TEST_CASE("GameSystem removing the active state leaves no active state", "[gamesystem]")
{
	GameSystem system;
	StateLog log;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&log)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	system.removeState("play");

	CHECK_FALSE(system.update(0.f));
}

TEST_CASE("GameSystem removing another state keeps the active state", "[gamesystem]")
{
	GameSystem system;
	StateLog activeLog;
	StateLog otherLog;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&activeLog)));
	REQUIRE_NOTHROW(system.addState("pause", new FakeState(&otherLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	system.removeState("pause");

	CHECK(system.update(0.f));
}
