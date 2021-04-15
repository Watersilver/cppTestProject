#include "Game.h"

#include <cmath>
#include <utility>

#include "ecsObj.h"
#include "components.h"
#include "collision.h"

using namespace ecs;
using namespace components;

void Game::SysPatrol::update(ecs::EntityId entityID, float deltaT)
{
  auto& pos = world.getComponent<Position>(entityID)->p;
  auto& vel = world.getComponent<Velocity>(entityID)->v;

  if (pos.y > 1000.0f)
  {
    world.addComponent<DeathCounter>(entityID);
    world.removeComponent<Patrol>(entityID);
  }

  // Only work if in screen
  auto& g = Game::getInstance();
  auto tlwrld = g.tv.ScreenToWorld({ -16, -16 });
  auto brwrld = g.tv.ScreenToWorld({ (int)g.s_width +16, (int)g.s_height + 16 });
  if (pos.x < tlwrld.x || pos.x > brwrld.x || pos.y < tlwrld.y || pos.y > brwrld.y)
  {
    if (vel.x != 0.1f) vel.x = 0.1f;
    else if (vel.x == 0.1f) vel.x = -0.1f;
    return;
  }

  auto& ppos = world.getComponent<PrevPosition>(entityID)->pp;
  auto& grounded = world.getComponent<Grounded>(entityID)->grounded;
  auto patrol = world.getComponent<Patrol>(entityID);

  bool turnAround = false;

  if (pos == ppos)
    turnAround = true;

  // Check for gap to avoid it
  if (grounded && !patrol->walk_off)
  {
    // Determine point that checks for gap based on my direction
    auto& bb = world.getComponent<BoundingBox>(entityID)->dimensions;
    olc::vf2d p{ pos.x + ((vel.x > 0.0f) ? 0.5f * (bb.x + 1.0f) : -0.5f * (bb.x + 1.0f)), pos.y + bb.y * 0.5f + 1.0f };

    auto& geo = world.getComponentPool<Tiles16>();
    for (auto& entry : geo)
    {
      auto& vTiles = world.getComponent<Tiles16>(entry.first)->tiles;
      auto tPosComp = world.getComponent<Position>(entry.first);

      olc::vf2d tOffset{};
      if (tPosComp) tOffset = tPosComp->p;

      std::pair<char, collision::Tile16> _;
      if (!collision::pointVsTiles(p, tOffset, vTiles, _))
      {
        turnAround = true;
        break;
      }
    }
  }

  if (turnAround)
    patrol->speed = -patrol->speed;

  vel.x = patrol->speed;
};
Signature Game::SysPatrol::sign{
  (world.getSignature<Patrol>()) |
  (world.getSignature<PrevPosition>()) |
  (world.getSignature<Position>()) |
  (world.getSignature<Grounded>()) |
  (world.getSignature<BoundingBox>()) |
  (world.getSignature<Velocity>())
};
