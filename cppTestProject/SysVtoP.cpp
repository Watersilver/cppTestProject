#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates position based on current velocity
void Game::SysVtoP::update(ecs::EntityId entityID, float deltaT)
{
	auto vel = world.getComponent<Velocity>(entityID);
	auto pos = world.getComponent<Position>(entityID);
	// v = dp / dt => dp = v * dt => p1 - p0 = v * dt => p1 = p0 + v * dt
  pos->p += (vel->v * deltaT);
};
Signature Game::SysVtoP::sign{
	(world.getSignature<Position>()) |
	(world.getSignature<Velocity>())
};
