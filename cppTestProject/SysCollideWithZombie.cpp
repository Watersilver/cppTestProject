#include "Game.h"

#include <cmath>
#include <utility>

#include "ecsObj.h"
#include "components.h"

using namespace ecs;
using namespace components;

void Game::SysCollideWithZombie::update(ecs::EntityId entityID, float deltaT)
{
  auto& vel = world.getComponent<Velocity>(entityID)->v;
  auto& pos = world.getComponent<Position>(entityID)->p;
  const auto& ppos = world.getComponent<PrevPosition>(entityID)->pp;
  const auto& bb = world.getComponent<BoundingBox>(entityID)->dimensions;
  auto ctrlp = world.getComponent<ControlParameters>(entityID);

  const auto zomPool = world.getComponentPool<Zombie>();

  for (auto& entry : zomPool)
  {
    const auto& zomID = entry.first;
    if (world.getComponent<Killed>(zomID))
      continue;

    const auto zomBB = world.getComponent<BoundingBox>(zomID);
    if (!zomBB)
      continue;

    const auto zomPos = world.getComponent<Position>(zomID);
    if (!zomPos)
      continue;

    // reduce self to a single point and expand enemy for easier equations.
    auto expPos = (zomPos->p - zomBB->dimensions * 0.5f) - bb * 0.5f;
    auto expSize = zomBB->dimensions + bb;

    // Check if I am inside target
    bool inside{
      pos.x > expPos.x && pos.x < expPos.x + expSize.x &&
      pos.y > expPos.y && pos.y < expPos.y + expSize.y
    };

    //Game::getInstance().tv.DrawRect(expPos, expSize);
    //Game::getInstance().tv.DrawCircle(zomPos->p, 10.0f, olc::RED);

    if (!inside)
      continue;

    // If I collide now and my lowest point was higher than its highest I stomp. Else I die!
    if (ppos.y < expPos.y + 3.0f) // Magic number to reduce harshness
    {
      // Stomp
      pos.y = expPos.y;
      vel.y = -20.0f;

      if (ctrlp)
      {
        ctrlp->activelyJumping = true;
      }

      world.addComponent<Killed>(zomID);
    }
    else
    {
      // Die
      world.addComponent<Killed>(entityID);
    }
  }
};
Signature Game::SysCollideWithZombie::sign{
  (world.getSignature<Player>()) |
  (world.getSignature<PrevPosition>()) |
  (world.getSignature<Position>()) |
  (world.getSignature<BoundingBox>()) |
  (world.getSignature<CollidesWithBoundingBox>()) |
  (world.getSignature<Velocity>())
};
