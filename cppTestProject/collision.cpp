#include "Game.h"

#include <utility>
#include <vector>
#include <cmath>
#include <iostream>

#include "collision.h"
#include "olcPixelGameEngine.h"

// Tile is represented by single point, because they are always 16 x 16
using Tile16 = collision::Tile16;

// colTile is tile16 that collides with point
bool collision::pointVsTiles(const olc::vf2d& point, const olc::vf2d& tilesOffset, const std::vector<std::string>& tiles, std::pair<char, Tile16>& colTile)
{
  // Convert point to tile index
  auto ind = (olc::vi2d)((point - tilesOffset) / 16).floor();

  // Check if point is within tiles
  if (ind.y < 0 || ind.y >= (int)tiles.size()) return false;
  if (ind.x < 0 || ind.x >= (int)tiles[0].size()) return false;

  if (tiles[ind.y][ind.x] != '.')
  {
    colTile.first = tiles[ind.y][ind.x];
    colTile.second = ((Tile16)ind) * 16.0 + tilesOffset;
    return true;
  }

  return false;
}

// colTiles are tiles16 that collide with rectangle
bool collision::rectVsTiles(const collision::rect& cRect, const olc::vf2d& tilesOffset, const std::vector<std::string>& tiles, std::vector<std::pair<char, Tile16>>& colTiles)
{
  auto startingSize = colTiles.size();

  // Convert points to tile index
  auto ind1 = (olc::vi2d)((cRect.pos - tilesOffset) / 16).floor();
  auto ind2 = (olc::vi2d)((cRect.pos + cRect.size - tilesOffset) / 16).floor();

  // tiles rows and columns
  auto tr = (std::int32_t)tiles.size();
  auto tc = (std::int32_t)tiles[0].size();

  // Check if any rect point is within tiles
  if ((ind1.x > tc - 1) || (ind1.y > tr - 1) || (ind2.x < 0) || (ind2.y < 0)) return false;

  // ind1 could be lesser than 0
  // but if it's greater than size,
  // we will never get here. So no need to clamp, just max
  ind1 = ind1.max(olc::vi2d{ 0, 0 });

  // Opposite from above comment for ind2
  ind2 = ind2.min(olc::vi2d{ (std::int32_t)tc - 1, (std::int32_t)tr - 1 });

  // Iterate possible tiles and return colliding tiles
  for (int row = ind1.y; row <= ind2.y; row++)
    for (int column = ind1.x; column <= ind2.x; column++)
    {
      // If I want to go through tiles mark here
      const auto& tile = tiles[row][column];
      // empty grass water cloud tree
      if (tile != '.' && tile != '^' && tile != 'w' && tile != 'v' && tile != '1' && tile != '2' && tile != '|')
      {
        colTiles.emplace_back(tiles[row][column], Tile16{ column * 16.0f + tilesOffset.x, row * 16.0f + tilesOffset.y});
      }
    }

  if (colTiles.size() > startingSize) return true;

  return false;
}


using rect = collision::rect;
bool collision::checkCollision(const rect& r, const olc::vf2d& vel, float dt, const rect& target, olc::vf2d& contact_point, olc::vf2d& contact_normal)
{
  auto center = r.pos + r.size * 0.5f;

  rect expanded_target{};
  expanded_target.pos = target.pos - r.size * 0.5f;
  expanded_target.size = target.size + r.size;

  // Check if I will be inside target
  auto c = center + vel * dt;
  bool inside{
    c.x > expanded_target.pos.x && c.x < expanded_target.pos.x + expanded_target.size.x &&
    c.y > expanded_target.pos.y && c.y < expanded_target.pos.y + expanded_target.size.y
  };

  if (!inside)
    return false;

  const float leftDis{ (center.x - expanded_target.pos.x) };
  const float rightDis{ std::abs(center.x - (expanded_target.pos.x + expanded_target.size.x)) };

  const float upDis{ std::abs(center.y - expanded_target.pos.y) };
  const float downDis{ std::abs(center.y - (expanded_target.pos.y + expanded_target.size.y)) };

  if (std::min(leftDis, rightDis) < std::min(upDis, downDis))
    if (leftDis < rightDis)
    {
      contact_normal = { -1.0f, 0.0 };
      contact_point.x = expanded_target.pos.x;
      contact_point.y = center.y;
    }
    else
    {
      contact_normal = { 1.0f, 0.0 };
      contact_point.x = expanded_target.pos.x + expanded_target.size.x;
      contact_point.y = center.y;
    }
  else
    if (upDis < downDis)
    {
      contact_normal = { 0.0f, -1.0 };
      contact_point.x = center.x;
      contact_point.y = expanded_target.pos.y;
    }
    else
    {
      contact_normal = { 0.0f, 1.0 };
      contact_point.x = center.x;
      contact_point.y = expanded_target.pos.y + expanded_target.size.y;
    }

  return true;
}
