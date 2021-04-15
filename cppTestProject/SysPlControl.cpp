#include "Game.h"

#include <vector>
#include <utility>
#include <cmath>
#include <random>

#include "olcPGEX_Sound.h"

#include "ecsObj.h"
#include "components.h"
#include "collision.h"

using namespace ecs;
using namespace components;

void Game::SysPlControl::update(ecs::EntityId entityID, float deltaT)
{
  auto gravPool = world.getComponentPool<Gravity>();
  if (gravPool.size() != 1) return;
  olc::vf2d grav = world.getComponent<Gravity>((*(gravPool.begin())).first)->g;

  auto chp = world.getComponentPool<Checkpoint>();
  if (chp.size() != 1) return;
  olc::vf2d& checkpoint = world.getComponent<Checkpoint>((*(chp.begin())).first)->p;

  auto& g = Game::getInstance();

  const auto pos = world.getComponent<Position>(entityID);
  if (pos)
    if (pos->p.x > 4144.0f)
    {
      if (checkpoint.x != 44.0f)
      {
        olc::SOUND::PlaySample(g.successSfx);
        checkpoint = { 44.0f, 44.0f };
        world.addComponent<DeathCounter>(entityID)->c = 4.0f;
        world.removeComponent<Controllable>(entityID);
      }
    }
    else if (pos->p.x > 2207.0f)
    {
      if (checkpoint.x < 2207.0f)
        checkpoint = { 2207.0f, 222.0f };
    }
    else if (pos->p.x > 1666.0f)
    {
      if (checkpoint.x < 1666.0f)
        checkpoint = { 1666.0f, 96.0f };
    }
    else if (pos->p.x > 752.0f)
    {
      if (checkpoint.x < 752.0f)
        checkpoint = { 752.0f, 143.0f };
    }

  const auto& grounded = world.getComponent<Grounded>(entityID)->grounded;
  auto& vel = world.getComponent<Velocity>(entityID)->v;
  auto& acc = world.getComponent<ClampedAccel>(entityID)->a;
  auto& deccx = world.getComponent<ClampedAccel>(entityID)->decceleratingx;

  auto cp = world.getComponent<ControlParameters>(entityID);

  // Handle jumping
  if (grounded && g.GetKey(olc::UP).bPressed)
  {
    cp->activelyJumping = true;
    // vel = olc::vf2d{ vel.x, -grav.y * 0.5f }; works well for grav 100
    vel = olc::vf2d{ vel.x, -70.0f };
    olc::SOUND::PlaySample(g.jumpSfx);
  }

  if (vel.y < 0.0f && cp->activelyJumping && g.GetKey(olc::UP).bHeld)
    acc.y = -grav.y * 0.65;
  else
    cp->activelyJumping = false;

  // Handle horizontall movement

  float ax;
  if (grounded)
    ax = 200.0f;
  else
    ax = 50.0f;

  float move_side{ 0.0f };

  if (g.GetKey(olc::LEFT).bHeld) move_side = -1.0f;
  if (g.GetKey(olc::RIGHT).bHeld) move_side = 1.0f;

  if (move_side != 0.0f)
    acc.x = ax * move_side;
  else if (grounded)
  {
    // slowdown
    if (vel.x > 0.0f)
      acc.x = -0.5f * ax;
    else
      acc.x = 0.5f * ax;
    deccx = true;
  }

  // Check if dead
  if (pos && pos->p.y > 1000.0f)
    world.addComponent<Killed>(entityID);
};
Signature Game::SysPlControl::sign{
  (world.getSignature<Player>()) |
  (world.getSignature<Controllable>()) |
  (world.getSignature<Velocity>()) |
  (world.getSignature<ClampedAccel>()) |
  (world.getSignature<ControlParameters>()) |
  (world.getSignature<Grounded>())
};
