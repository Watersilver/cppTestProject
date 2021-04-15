#include "Game.h"

#include <utility>
#include <algorithm>
#include <cmath>

#include "ecsObj.h"
#include "components.h"
#include "collision.h"

using namespace ecs;
using namespace components;

// Hacky collision resolution
void Game::SysResolveStaticColls::update(ecs::EntityId entityID, float deltaT)
{
  auto& pos = world.getComponent<Position>(entityID)->p;
  auto& vel = world.getComponent<Velocity>(entityID)->v;
  auto& bb = world.getComponent<BoundingBox>(entityID)->dimensions;
  auto& cols = world.getComponent<CollidesWithStatic>(entityID)->potentialColls;

  auto grounded = world.getComponent<Grounded>(entityID);
  if (grounded) grounded->grounded = false;

  olc::vf2d contact_point, contact_normal;

  // Resolve collisions closest to farthest
  std::vector<std::pair<int, float>> cOrder;

  collision::rect dyn{ pos - bb * 0.5f, bb };

  unsigned int i{ 0 };
  auto center = dyn.pos + dyn.size * 0.5f;
  for (auto& r : cols)
  {
    if (checkCollision(dyn, vel, deltaT, collision::rect{ r.second, {16, 16} }, contact_point, contact_normal))
    {
      cOrder.emplace_back(i, (contact_point - center).mag2());
    }
    i++;
  }

  std::sort(cOrder.begin(), cOrder.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
    return a.second < b.second;
    });

  for (auto c : cOrder)
    if (checkCollision(dyn, vel, deltaT, collision::rect{ cols[c.first].second, {16, 16} }, contact_point, contact_normal))
    {
      // dyn.pos = contact_point - dyn.size * 0.5f;
      pos = contact_point;
      vel = vel * olc::vf2d{ std::abs(contact_normal.y), std::abs(contact_normal.x) };

      if (grounded)
      {
        if (contact_normal.y < 0.0f)
          grounded->grounded = true;
      }
    }
};
Signature Game::SysResolveStaticColls::sign{
  (world.getSignature<CollidesWithStatic>()) |
  (world.getSignature<Position>()) |
  (world.getSignature<Velocity>()) |
  (world.getSignature<BoundingBox>())
};
