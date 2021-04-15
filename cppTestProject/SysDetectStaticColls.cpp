#include "Game.h"

#include <utility>
#include <cmath>

#include "ecsObj.h"
#include "components.h"
#include "collision.h"

using namespace ecs;
using namespace components;

void Game::SysDetectStaticColls::update(ecs::EntityId entityID, float deltaT)
{
  const auto& pos = world.getComponent<Position>(entityID)->p;
  const auto& vel = world.getComponent<Velocity>(entityID)->v;
  const auto& bb = world.getComponent<BoundingBox>(entityID)->dimensions;

  auto& g = Game::getInstance();

  // Create a rect that contains both current and
  // what future pos would be without collisions.
  // Pad it out a little too.
  auto topLeft = pos - bb * 0.5;
  topLeft = topLeft.min(topLeft + vel * (float)deltaT);
  auto size = bb.max(bb + olc::vf2d{ std::abs(vel.x * (float)deltaT), std::abs(vel.y * (float)deltaT) });
  topLeft -= olc::vf2d{ 1, 1 };
  size += olc::vf2d{ 2, 2 };

  //g.tv.DrawRect(topLeft, size);

  //g.tv.DrawRect(pos - bb * 0.5, bb, olc::RED);

  //auto collidingTiles = std::vector<std::pair<char, collision::Tile16>>{};
  auto& collidingTiles = world.getComponent<CollidesWithStatic>(entityID)->potentialColls;
  collidingTiles.clear();

  auto& geo = world.getComponentPool<Tiles16>();
  for (auto& entry : geo)
  {
    auto& vTiles = world.getComponent<Tiles16>(entry.first)->tiles;
    auto tPosComp = world.getComponent<Position>(entry.first);

    olc::vf2d tOffset{};
    if (tPosComp) tOffset = tPosComp->p;

    // Make rect based on both pos and vel
    //if (collision::rectVsTiles(topLeft, bb, tOffset, tiles, collidingTiles))
    collision::rectVsTiles(collision::rect{ topLeft, size }, tOffset, vTiles, collidingTiles);
  }
};
Signature Game::SysDetectStaticColls::sign{
  (world.getSignature<CollidesWithStatic>()) |
  (world.getSignature<Position>()) |
  (world.getSignature<Velocity>()) |
  (world.getSignature<BoundingBox>())
};
