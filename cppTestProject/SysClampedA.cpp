#include "Game.h"

#include <cmath>

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysClampedA::update(ecs::EntityId entityID, float deltaT)
{
  auto cacc = world.getComponent<ClampedAccel>(entityID);
  auto& vel = world.getComponent<Velocity>(entityID)->v;
  // a = dv / dt => dv = a * dt => v1 - v0 = a * dt => v1 = v0 + a * dt

  float prevx = vel.x;
  float prevy = vel.y;

  if (cacc->maxspeed.x <= 0.0f || cacc->maxspeed.x > std::abs(vel.x + cacc->a.x * deltaT))
    vel.x += cacc->a.x * deltaT;
  else
    vel.x = cacc->maxspeed.x * (-(float)std::signbit(vel.x) * 2.0f + 1.0f);

  if (cacc->maxspeed.y <= 0.0f || cacc->maxspeed.y > std::abs(vel.y + cacc->a.y * deltaT))
    vel.y += cacc->a.y * deltaT;
  else
    vel.y = cacc->maxspeed.y * (-(float)std::signbit(vel.y) * 2.0f + 1.0f);

  if (cacc->decceleratingx && prevx * vel.x <= 0)
    vel.x = 0;
  if (cacc->decceleratingy && prevy * vel.y <= 0)
    vel.y = 0;

  cacc->a = { 0.0f, 0.0f };
  cacc->decceleratingx = false;
  cacc->decceleratingy = false;
};
Signature Game::SysClampedA::sign{
  (world.getSignature<ClampedAccel>()) |
  (world.getSignature<Velocity>())
};
