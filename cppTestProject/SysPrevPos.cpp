#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// Store previous position
void Game::SysPrevPos::update(ecs::EntityId entityID, float deltaT)
{
  world.getComponent<PrevPosition>(entityID)->pp = world.getComponent<Position>(entityID)->p;
};
Signature Game::SysPrevPos::sign{
  (world.getSignature<Position>()) |
  (world.getSignature<PrevPosition>())
};
