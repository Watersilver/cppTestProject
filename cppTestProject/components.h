#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <vector>
#include <utility>

#include "olcPixelGameEngine.h"
#include "Ecs.h"

namespace components
{
	struct Position : Component
	{
		olc::vf2d p = { 0.0f, 0.0f };
	};

  struct PrevPosition : Component
  {
    olc::vf2d pp = { 0.0f, 0.0f };
  };

	struct Velocity : Component
	{
		olc::vf2d v = { 0.0f, 0.0f };
	};

	struct Acceleration : Component
	{
		olc::vf2d a = { 0.0f, 0.0f };
	};

  struct ClampedAccel : Component
  {
    olc::vf2d a = { 0.0f, 0.0f };
    olc::vf2d maxspeed = { 50.0f, -1.0f };
    bool decceleratingx{ false };
    bool decceleratingy{ false };
  };

	struct Scale : Component
	{
		olc::vf2d s = { 0.0f, 0.0f };
	};

	struct Angle : Component
	{
    float rad{};
    float angVel{};
	};

  struct BoundingBox : Component
  {
    olc::vf2d dimensions{ 9.0f, 15.6f };
  };

  struct Tiles16 : Component
  {
    std::vector<std::string> tiles{};
  };

  // camerabounds unique and stack
  // same with camera target
  struct Camera : UniqueComponent
  {
    EntityConnection target;
    olc::vf2d tl{ 0.0f, -10000.0f };
    olc::vf2d br{10000.0f, 255.0f};
  };

  struct Grounded : Component
  {
    bool grounded{};
  };

  // Global gravity
  struct Gravity : UniqueComponent
  {
    //olc::vf2d g = { 0.0f, 100.0f };
    olc::vf2d g = { 0.0f, 300.0f };
  };

  // Local gravity
  struct GravityFactor : Component
  {
    olc::vf2d f = { 1.0f, 1.0f };
  };

  struct CollidesWithStatic : Component
  {
    std::vector<std::pair<char, olc::vf2d>> potentialColls{};
  };
  struct CollidesWithBoundingBox : Component
  {
  
  };

  struct ControlParameters : Component
  {
    bool activelyJumping{false};
  };

  struct Animation : Component
  {
    int prevState{-1};
    int state{};
    float direction{0.0f};
    float progress{}; // 0 - 1 float
    float speed{};
  };

  struct DeathCounter : Component
  {
    float c{ 0.0f };
  };

  struct Checkpoint : UniqueComponent
  {
    olc::vf2d p{ 44.0f, 44.0f };
  };

  struct Patrol : Component
  {
    float speed{-10.0f};
    bool walk_off{ false };
  };

	// Tags
  struct Player : UniqueComponent {};
  struct Controllable : Component {};
  struct Zombie : Component {};
  struct Killed : Component {};
  struct Visible : Component {};
}

#endif