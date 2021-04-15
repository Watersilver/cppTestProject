#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current gravity
void Game::SysApplyGrav::update(ecs::EntityId entityID, float deltaT)
{
  auto grav = world.getComponentPool<Gravity>();
  if (grav.size() != 1) return;

  auto& vel = world.getComponent<Velocity>(entityID)->v;
  auto& gfactor = world.getComponent<GravityFactor>(entityID)->f;

  for (auto& entry : grav)
  {
    auto& g = world.getComponent<Gravity>(entry.first)->g;
    vel += (g * gfactor) * deltaT;
  }
};
Signature Game::SysApplyGrav::sign{
  (world.getSignature<GravityFactor>()) |
  (world.getSignature<Velocity>())
};
