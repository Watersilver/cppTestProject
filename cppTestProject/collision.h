#ifndef COLLISION_H
#define COLLISION_H

// Ray:
// Nx < Fy
// Ny < Fx

// point = max(Nx, Ny);

#include <utility>

#include "olcPixelGameEngine.h"

namespace collision
{
  // Tile is represented by single point, because they are always 16 x 16
  using Tile16 = olc::vf2d;

  struct rect
  {
    olc::vf2d pos;
    olc::vf2d size;
  };

  // colTile is tile16 that collides with point
  bool pointVsTiles(const olc::vf2d&, const olc::vf2d&, const std::vector<std::string>&, std::pair<char, Tile16>&);

  // colTiles are tiles16 that collide with rectangle
  bool rectVsTiles(const rect&, const olc::vf2d&, const std::vector<std::string>&, std::vector<std::pair<char, Tile16>>&);

  bool checkCollision(const rect&, const olc::vf2d&, float, const rect&, olc::vf2d&, olc::vf2d&);
}

#endif