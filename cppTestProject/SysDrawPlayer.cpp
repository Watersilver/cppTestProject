#include "Game.h"

#include <cmath>
#include <string>

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

enum aState
{
  walking,
  jumping,
  falling,
  idle
};

// This system calculates velocity based on current acceleration
void Game::SysDrawPlayer::update(ecs::EntityId entityID, float deltaT)
{
  auto& pos = world.getComponent<Position>(entityID)->p;
  auto angleComp = world.getComponent<Angle>(entityID);
  auto anim = world.getComponent<Animation>(entityID);

  float angle{ 0 };

  if (angleComp)
  {
    angle = angleComp->rad;
  }

  auto& g = Game::getInstance();

  // Determine animation state
  auto gr = world.getComponent<Grounded>(entityID);
  auto vel = world.getComponent<Velocity>(entityID);
  if (world.getComponent<Killed>(entityID))
    anim->state = falling;
  else if (gr && gr->grounded)
  {
    if (vel)
      anim->speed = std::abs(vel->v.x) > 0.01f ? (std::abs(vel->v.x) > 35.0f ? 5.0f : 2.0f) : 0.0f;
    else
      anim->speed = 0.0f;

    if (anim->speed == 0)
      anim->state = idle;
    else
      anim->state = walking;

    if (anim->state != anim->prevState)
      anim->progress = 0.0f;
  }
  else
  {
    anim->speed = 0.0f;
    if (vel && vel->v.y > 0.0f)
      anim->state = falling;
    else
      anim->state = jumping;
  }

  anim->progress = (anim->progress + anim->speed * deltaT) - (float)((int)anim->progress); // Remain within [1, 0)

  anim->prevState = anim->state;

  if (world.getComponent<Controllable>(entityID))
    if (g.GetKey(olc::RIGHT).bHeld)
      anim->direction = 0.0f;
    else if (g.GetKey(olc::LEFT).bHeld)
      anim->direction = 1.0f;

  olc::vf2d scale{ 1.0f - anim->direction * 2.0f, 1.0f };

  if (anim->state == walking)
    g.tv.DrawPartialRotatedDecal(pos, g.plWalk.Decal(), angle, olc::vf2d{ 8.0f, 8.0f }, olc::vf2d{ 16.0f * (anim->progress < 0.5f), 0.0f }, olc::vf2d{ 16.0f, 16.0f }, scale);
  else if (anim->state == jumping)
    g.tv.DrawPartialRotatedDecal(pos, g.plJump.Decal(), angle, olc::vf2d{ 8.0f, 8.0f }, olc::vf2d{ 0.0f, 0.0f }, olc::vf2d{ 16.0f, 16.0f }, scale);
  else if (anim->state == idle)
    g.tv.DrawPartialRotatedDecal(pos, g.plIdle.Decal(), angle, olc::vf2d{ 8.0f, 8.0f }, olc::vf2d{ 0.0f, 0.0f }, olc::vf2d{ 16.0f, 16.0f }, scale);
  else if (anim->state == falling)
    g.tv.DrawPartialRotatedDecal(pos, g.plJump.Decal(), angle, olc::vf2d{ 8.0f, 8.0f }, olc::vf2d{ 16.0f, 0.0f }, olc::vf2d{ 16.0f, 16.0f }, scale);
};
Signature Game::SysDrawPlayer::sign{
  (world.getSignature<Position>()) |
  (world.getSignature<Visible>()) |
  (world.getSignature<Player>()) |
  (world.getSignature<Animation>())
};
