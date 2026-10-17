#include "NullRenderSystem.h"
#include "NullSoundSystem.h"

#include "collision/CollisionSystem.h"
#include "game/GameSystem.h"
#include "game/state/LoadState.h"
#include "game/state/LoseState.h"
#include "game/state/PauseState.h"
#include "game/state/WinState.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadState plays the loading music again after being re-entered", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;
	GameSystem gameSystem;

	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem);
	state.init();
	state.onEnter();
	REQUIRE(state.update(0.016f));
	REQUIRE(soundSystem.d_bgmPlays.size() == 1);
	CHECK(soundSystem.d_bgmPlays[0] == "loading");

	// A run leaves this state with the music started. Coming back has to start
	// it over, otherwise the startup screen is silent for the rest of the session.
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

	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem);
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

	LoadState state(&renderSystem, &collisionSystem, &soundSystem, &gameSystem);
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

	WinState state(&renderSystem, &collisionSystem, &soundSystem);
	state.init();

	state.onEnter();
	state.onExit();
	state.onEnter();

	// onEnter starts the music every time, unlike the load state which tracks
	// whether it already did.
	CHECK(soundSystem.d_bgmPlays.size() == 2);
	CHECK(soundSystem.d_bgmPlays[0] == "win");
	CHECK(soundSystem.d_bgmPlays[1] == "win");
}

TEST_CASE("LoseState plays its music every time it is entered", "[gamestate]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	CollisionSystem collisionSystem;

	LoseState state(&renderSystem, &collisionSystem, &soundSystem);
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

	WinState win(&renderSystem, &collisionSystem, &soundSystem);
	win.init();
	LoseState lose(&renderSystem, &collisionSystem, &soundSystem);
	lose.init();

	CHECK(renderSystem.d_createdScenes.size() == 2);
	CHECK(renderSystem.d_createdScenes[0] == "win");
	CHECK(renderSystem.d_createdScenes[1] == "lose");
	// A different color per screen is the whole point, so each state has to set
	// its own rather than inheriting whatever was there before.
	CHECK(renderSystem.d_backgroundColorCount == 2);
	CHECK(renderSystem.d_lastBackgroundScene == "lose");
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