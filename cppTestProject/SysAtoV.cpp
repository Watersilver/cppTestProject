#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysAtoV::update(ecs::EntityId entityID, float deltaT)
{
	auto& acc = world.getComponent<Acceleration>(entityID)->a;
	auto& vel = world.getComponent<Velocity>(entityID)->v;
	// a = dv / dt => dv = a * dt => v1 - v0 = a * dt => v1 = v0 + a * dt

	vel += acc * deltaT;

  acc = { 0, 0 };
};
Signature Game::SysAtoV::sign{
	(world.getSignature<Velocity>()) |
	(world.getSignature<Acceleration>())
};
