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
		bool collisions = true;
		bool throwOnInit = false;
		int destructions = 0;
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

		~FakeState() override
		{
			++d_log->destructions;
		}

		void init() override
		{
			if (d_log->throwOnInit)
			{
				throw std::runtime_error("init failed");
			}
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

		bool collisionsEnabled() const override
		{
			return d_log->collisions;
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

TEST_CASE("GameSystem reports whether the active state wants collisions", "[gamesystem]")
{
	GameSystem system;
	StateLog playLog;
	StateLog pauseLog;
	REQUIRE_NOTHROW(system.addState("play", new FakeState(&playLog)));
	REQUIRE_NOTHROW(system.addState("pause", new FakeState(&pauseLog)));

	ConfigFile config;
	config.set("game_initialstate", "play");
	REQUIRE_NOTHROW(system.init(config));

	CHECK(system.collisionsEnabled());

	// Pause must stop collisions, or damage keeps accumulating in a frozen world.
	pauseLog.collisions = false;
	playLog.updateResult = false;
	playLog.nextState = "pause";
	REQUIRE(system.update(0.f));

	CHECK_FALSE(system.collisionsEnabled());
}

TEST_CASE("GameSystem disables collisions when there is no active state", "[gamesystem]")
{
	GameSystem system;

	CHECK_FALSE(system.collisionsEnabled());
}

TEST_CASE("GameSystem takes ownership of a state whose init throws", "[gamesystem]")
{
	GameSystem system;
	StateLog log;
	log.throwOnInit = true;

	CHECK_THROWS(system.addState("play", new FakeState(&log)));

	// The state must be owned before init runs, or a throwing init leaks it.
	CHECK(log.destructions == 1);
}

TEST_CASE("GameSystem exits the active state when it is destroyed", "[gamesystem]")
{
	StateLog log;
	{
		GameSystem system;
		REQUIRE_NOTHROW(system.addState("play", new FakeState(&log)));

		ConfigFile config;
		config.set("game_initialstate", "play");
		REQUIRE_NOTHROW(system.init(config));
	}

	CHECK(log.exits == 1);
	CHECK(log.destructions == 1);
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
