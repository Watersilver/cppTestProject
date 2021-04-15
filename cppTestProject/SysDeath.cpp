#include "Game.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysDeath::update(ecs::EntityId entityID, float deltaT)
{
  auto d = world.getComponent<DeathCounter>(entityID);

  d->c -= deltaT;

  if (d->c < 0)
  {
    world.deleteEntity(entityID);

    if (world.getComponent<Player>(entityID))
    {
      Game::getInstance().initialize();
    }
  }
};
Signature Game::SysDeath::sign{
  (world.getSignature<DeathCounter>())
};
