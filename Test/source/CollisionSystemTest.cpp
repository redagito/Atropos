#include "collision/CollisionSystem.h"

#include "collision/AABBox.h"
#include "collision/Collidable.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

TEST_CASE("CollisionSystem ignores removing a null entity", "[collision]")
{
	CollisionSystem system;

	CHECK_NOTHROW(system.remove(nullptr));
}

TEST_CASE("CollisionSystem defers entity removal to update", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();
	Collidable* entity = system.add(AABBox(), group);

	system.remove(entity);

	REQUIRE(entity->deleteRequested());
	CHECK_NOTHROW(system.update());
}

TEST_CASE("CollisionSystem deals damage between colliding groups", "[collision]")
{
	CollisionSystem system;
	const unsigned int groupA = system.getNewGroupId();
	const unsigned int groupB = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	Collidable* entityA = system.add(box, groupA);
	Collidable* entityB = system.add(box, groupB);
	entityA->setDamage(5.f);

	system.update();

	CHECK(entityB->getDamageReceived() == 5.f);
	CHECK(entityA->getDamageReceived() == 0.f);
}

TEST_CASE("CollisionSystem deals damage once per contact", "[collision]")
{
	CollisionSystem system;
	const unsigned int groupA = system.getNewGroupId();
	const unsigned int groupB = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	Collidable* entityA = system.add(box, groupA);
	Collidable* entityB = system.add(box, groupB);
	entityA->setDamage(5.f);

	system.update();
	REQUIRE(entityB->getDamageReceived() == 5.f);

	// The two still overlap, so this is the same contact and must not deal
	// damage again. Applying it every frame would make the damage dealt depend
	// on the frame rate.
	system.update();
	CHECK(entityB->getDamageReceived() == 0.f);
	system.update();
	CHECK(entityB->getDamageReceived() == 0.f);
}

TEST_CASE("CollisionSystem deals damage again after a contact ends", "[collision]")
{
	CollisionSystem system;
	const unsigned int groupA = system.getNewGroupId();
	const unsigned int groupB = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	Collidable* entityA = system.add(box, groupA);
	Collidable* entityB = system.add(box, groupB);
	entityA->setDamage(5.f);

	system.update();
	REQUIRE(entityB->getDamageReceived() == 5.f);

	// Separate, the contact closes and no further damage is dealt.
	entityB->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 100.f)));
	system.update();
	CHECK(entityB->getDamageReceived() == 0.f);

	// Touching again is a new contact, so the damage applies once more.
	entityB->setTranslation(glm::mat4(1.f));
	system.update();
	CHECK(entityB->getDamageReceived() == 5.f);
}

TEST_CASE("CollisionSystem deals damage to a collidable that reuses a freed address", "[collision]")
{
	CollisionSystem system;
	const unsigned int groupA = system.getNewGroupId();
	const unsigned int groupB = system.getNewGroupId();

	AABBox box;
	box.setHalfWidths(glm::vec3(1.f));

	Collidable* target = system.add(box, groupB);
	Collidable* dealer = system.add(box, groupA);
	dealer->setDamage(5.f);

	system.update();
	REQUIRE(target->getDamageReceived() == 5.f);

	// The dealer is freed while still touching the target. A new collidable
	// allocated at the same address must open a fresh contact instead of
	// inheriting the old one and skipping its damage.
	system.remove(dealer);
	system.update();
	CHECK(target->getDamageReceived() == 0.f);

	Collidable* replacement = system.add(box, groupA);
	replacement->setDamage(5.f);

	system.update();
	CHECK(target->getDamageReceived() == 5.f);
}

TEST_CASE("CollisionSystem reuses released group ids", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();
	system.add(AABBox(), group);

	system.releaseGroup(group);

	CHECK(system.getNewGroupId() == group);
}

TEST_CASE("CollisionSystem ignores invalid or repeated group releases", "[collision]")
{
	CollisionSystem system;
	const unsigned int group = system.getNewGroupId();

	CHECK_NOTHROW(system.releaseGroup(999));
	system.releaseGroup(group);
	CHECK_NOTHROW(system.releaseGroup(group));

	// The id is handed back out only once
	CHECK(system.getNewGroupId() == group);
	CHECK(system.getNewGroupId() == group + 1);
}
