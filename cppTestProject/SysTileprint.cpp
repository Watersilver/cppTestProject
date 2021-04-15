#include "Game.h"

#include <cmath>
#include <string>

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

// This system calculates velocity based on current acceleration
void Game::SysTileprint::update(ecs::EntityId entityID, float deltaT)
{
  auto vTiles = world.getComponent<Tiles16>(entityID)->tiles;

  if (!vTiles.size())
    return;

  const auto& pos = world.getComponent<Position>(entityID)->p;
  const auto& aSpeed = world.getComponent<Animation>(entityID)->speed;
  auto& aProgress = world.getComponent<Animation>(entityID)->progress;

  aProgress = (aProgress + aSpeed * deltaT) - (float)((int)aProgress); // Remain within [1, 0)

  auto& g = Game::getInstance();

  // Only draw tiles I need to
  auto tlwrld = g.tv.ScreenToWorld({ 0, 0 }) - pos;
  auto brwrld = g.tv.ScreenToWorld({ (int)g.s_width + 16, (int)g.s_height + 16 }) - pos;

  float div16 = 1.0f / 16.0f;
  int minCol = std::max(0, std::min((int)vTiles[0].size(), (int)(tlwrld.x * div16)));
  int maxCol = std::max(0, std::min((int)vTiles[0].size(), (int)(brwrld.x * div16)));
  int minRow = std::max(0, std::min((int)vTiles.size(), (int)(tlwrld.y * div16)));
  int maxRow = std::max(0, std::min((int)vTiles.size(), (int)(brwrld.y * div16)));
  for (auto c = minCol; c != maxCol; c++)
  {
    for (auto r = minRow; r != maxRow; r++)
    {
      auto tile = vTiles[r][c];
      // If I want other kinds of tiles to be printed, here we are.
      // don't forget to add padding to position
      if (tile == '#') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 0.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '^') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 7.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == 'w') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ (aProgress < 0.5f ? 3.0f : 4.0f) * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == 'v') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ (aProgress < 0.5f ? 5.0f : 6.0f) * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '1') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 12.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '2') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 13.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '|') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 11.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '[') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 8.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '=') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 9.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == ']') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 10.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == '@') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 1.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
      if (tile == 'o') g.tv.DrawPartialDecal(olc::vf2d{ (float)c * 16.0f + pos.x - 1.0f, (float)r * 16.0f + pos.y - 1.0f }, g.tiles.Decal(), olc::vf2d{ 2.0f * 18.0f, 0.0f }, olc::vf2d{ 18.0f, 18.0f });
    }
  }
};
Signature Game::SysTileprint::sign{
  (world.getSignature<Position>()) |
  (world.getSignature<Tiles16>()) |
  (world.getSignature<Animation>()) |
  (world.getSignature<Visible>())
};
