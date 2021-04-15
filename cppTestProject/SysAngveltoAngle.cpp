#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysAngveltoAngle::update(ecs::EntityId entityID, float deltaT)
{
  auto ang = world.getComponent<Angle>(entityID);

  ang->rad += ang->angVel * deltaT;
};
Signature Game::SysAngveltoAngle::sign{
  (world.getSignature<Angle>())
};
