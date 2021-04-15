#include "Game.h"

#include <cmath>

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysCamera::update(ecs::EntityId entityID, float deltaT)
{
  auto cam = world.getComponent<Camera>(entityID);
  auto& target = cam->target;
  const auto& tl = cam->tl;
  const auto& br = cam->br;

  if (target.id() == 0) return;

  auto tPos = world.getComponent<Position>(target.id());

  if (!tPos) return;

  auto& g = Game::getInstance();

  auto camCenter = tPos->p;
  auto scaledScrCenter = g.tv.ScaleToWorld(olc::vf2d(g.ScreenWidth() * 0.5f, g.ScreenHeight() * 0.5f));

  camCenter.x = std::max(tl.x + scaledScrCenter.x, camCenter.x);
  camCenter.y = std::min(br.y - scaledScrCenter.y, camCenter.y);
  // Don't really care about top and right limits, so don't implement for now

  g.tv.SetWorldOffset(camCenter - scaledScrCenter);
};
Signature Game::SysCamera::sign{
  (world.getSignature<Camera>())
};
