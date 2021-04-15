#include "Game.h"

#include "olcPGEX_Sound.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

void Game::SysKilled::update(ecs::EntityId entityID, float deltaT)
{
  if (world.getComponent<DeathCounter>(entityID))
    return;

  auto& cam = world.getComponentPool<Camera>();
  if (cam.size() > 0)
  {
    auto& camTarget = world.getComponent<Camera>((*(cam.begin())).first)->target;
    if (camTarget.id() == entityID)
      camTarget.setId(0);
  }
  world.removeComponent<Controllable>(entityID);
  world.removeComponent<CollidesWithStatic>(entityID);
  world.removeComponent<CollidesWithBoundingBox>(entityID);
  world.addComponent<DeathCounter>(entityID)->c = 3.0f;
  auto ang = world.getComponent<Angle>(entityID);
  if (ang)
  {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = 40.0f;
    float r = random * diff;
    ang->angVel = -20.0f + r;
  }
  auto vel = world.getComponent<Velocity>(entityID);
  if (vel)
  {
    vel->v.x = -30.0f + 60.0f * ((float)rand()) / (float)RAND_MAX;
    vel->v.y = -60.0f * ((float)rand()) / (float)RAND_MAX;
  }
  auto grded = world.getComponent<Grounded>(entityID);
  if (grded)
    grded->grounded = false;
  olc::SOUND::PlaySample(Game::getInstance().stompSfx);
};
Signature Game::SysKilled::sign{
  (world.getSignature<Killed>())
};
