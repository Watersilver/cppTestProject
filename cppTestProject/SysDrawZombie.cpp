#include "Game.h"

#include <cmath>

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

void Game::SysDrawZombie::update(ecs::EntityId entityID, float deltaT)
{
  auto& pos = world.getComponent<Position>(entityID)->p;
  auto angleComp = world.getComponent<Angle>(entityID);
  auto anim = world.getComponent<Animation>(entityID);

  float angle{ 0 };

  if (angleComp)
    angle = angleComp->rad;

  auto& g = Game::getInstance();

  // Determine animation state
  float facing{};
  auto prevp = world.getComponent<PrevPosition>(entityID)->pp;
  auto dp = pos - prevp;
  if (dp.x > 0.0f)
  {
    anim->direction = 1.0f;
  }
  else
  {
    anim->direction = 0.0f;
  }
  anim->speed = (dp.x == 0.0f || world.getComponent<Killed>(entityID)) ? 0.0f : 1.5f;

  anim->progress = (anim->progress + anim->speed * deltaT) - (float)((int)anim->progress); // Remain within [1, 0)

  olc::vf2d scale{ 1.0f - anim->direction * 2.0f, 1.0f };

  g.tv.DrawPartialRotatedDecal(pos, g.enZombie.Decal(), angle, olc::vf2d{ 9.0f, 9.0f }, olc::vf2d{ 18.0f * (anim->progress < 0.5f), 0.0f }, olc::vf2d{ 18.0f, 18.0f }, scale);
};
Signature Game::SysDrawZombie::sign{
  (world.getSignature<Position>()) |
  (world.getSignature<PrevPosition>()) |
  (world.getSignature<Visible>()) |
  (world.getSignature<Zombie>()) |
  (world.getSignature<Animation>())
};
