#include "NullRenderSystem.h"
#include "NullSoundSystem.h"

#include "collision/CollisionSystem.h"
#include "game/GameSystem.h"
#include "game/state/LoadState.h"
#include "game/state/LoseState.h"
#include "game/state/PauseState.h"
#include "game/state/WinState.h"
#include "game/RunStats.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadState plays the loading music again after being re-entered", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;
	GameSystem gameSystem;

	RunStats runStats;
	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem, runStats);
	state.init();
	state.onEnter();
	REQUIRE(state.update(0.016f));
	REQUIRE(soundSystem.d_bgmPlays.size() == 1);
	CHECK(soundSystem.d_bgmPlays[0] == "loading");

	// Coming back has to start the music over, or the startup screen stays silent.
	state.onExit();
	state.onEnter();
	REQUIRE(state.update(0.016f));

	CHECK(soundSystem.d_bgmPlays.size() == 2);
}

TEST_CASE("LoadState asks for every sound it needs on the first visit", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;
	GameSystem gameSystem;

	RunStats runStats;
	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem, runStats);
	state.init();

	// The later states reuse these, so they are requested up front.
	CHECK(soundSystem.d_bgmLoads.size() == 2);
	CHECK(soundSystem.d_bgmLoads[0] == "loading");
	CHECK(soundSystem.d_bgmLoads[1] == "combat");
	CHECK(soundSystem.d_sfxLoads.size() == 2);
	CHECK(renderSystem.d_requestedModels.size() == 4);
}

TEST_CASE("LoadState gives the startup screen its own background color", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;
	GameSystem gameSystem;

	RunStats runStats;
	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem, runStats);
	state.init();

	REQUIRE(renderSystem.d_createdScenes.size() == 1);
	CHECK(renderSystem.d_createdScenes[0] == "load");
	CHECK(renderSystem.d_lastBackgroundScene == "load");
}

TEST_CASE("WinState plays its music every time it is entered", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;

	RunStats runStats;
	WinState state(&renderSystem, &collisionSystem, &soundSystem, runStats);
	state.init();

	state.onEnter();
	state.onExit();
	state.onEnter();

	// Unlike the load state, onEnter starts the music unconditionally.
	CHECK(soundSystem.d_bgmPlays.size() == 2);
	CHECK(soundSystem.d_bgmPlays[0] == "win");
	CHECK(soundSystem.d_bgmPlays[1] == "win");
}

TEST_CASE("LoseState plays its music every time it is entered", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;

	RunStats runStats;
	LoseState state(&renderSystem, &collisionSystem, &soundSystem, runStats);
	state.init();

	state.onEnter();
	state.onExit();
	state.onEnter();

	CHECK(soundSystem.d_bgmPlays.size() == 2);
	CHECK(soundSystem.d_bgmPlays[0] == "lose");
}

TEST_CASE("Win and lose give their screens their own background color", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;

RunStats runStats;
	WinState win(&renderSystem, &collisionSystem, &soundSystem, runStats);
	win.init();
	LoseState lose(&renderSystem, &collisionSystem, &soundSystem, runStats);
	lose.init();

	CHECK(renderSystem.d_createdScenes.size() == 2);
	CHECK(renderSystem.d_createdScenes[0] == "win");
	CHECK(renderSystem.d_createdScenes[1] == "lose");
	// One shared color would put one screen on both, so each state sets its own.
	CHECK(renderSystem.d_backgroundColorCount == 2);
	CHECK(renderSystem.d_lastBackgroundScene == "lose");
}

TEST_CASE("Only the screen that asks for one gets a sky", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;
	GameSystem gameSystem;

	RunStats runStats;
	LoadState load(&renderSystem, &collisionSystem, &soundSystem, &gameSystem, runStats);
	load.init();
	WinState win(&renderSystem, &collisionSystem, &soundSystem, runStats);
	win.init();
	LoseState lose(&renderSystem, &collisionSystem, &soundSystem, runStats);
	lose.init();

	// None of them request one, so the renderer keeps its default and none of them
	// can inherit a sky another screen switched on.
	CHECK(renderSystem.d_skyEnabled == false);
	CHECK(renderSystem.d_skyScene.empty());
}

TEST_CASE("PauseState freezes the world but keeps the menu keys", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;

	PauseState state(&renderSystem, &collisionSystem, &soundSystem);
	state.init();

	CHECK_FALSE(state.collisionsEnabled());
	CHECK(state.update(0.016f));
}