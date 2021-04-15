#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

#include "globals.h"

#include "ecsObj.h"

using namespace ecs;

// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
public:
	static Game& getInstance() {
		static Game theInstance;
		return theInstance;
	}

  // Keep in mind in case I want to use the TileTransformedView subclass
  //olc::TileTransformedView tv;
  olc::TransformedView tv;

  const unsigned int s_width{160};
  const unsigned int s_height{144};

private:
	// Systems
  class SysDeath : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysPrevPos : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysClampedA : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysAtoV : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysApplyGrav : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  // Detect and resolve collision
  // using pos and vel using raycasting
  class SysDetectStaticColls : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysResolveStaticColls : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysVtoP : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysAngveltoAngle : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysCollideWithZombie : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysPatrol : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysPlControl : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysKilled : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  class SysCamera : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysTileprint : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysDrawZombie : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };
  class SysDrawPlayer : public System
  {
    void update(ecs::EntityId, float) override;
  public:
    static Signature sign;
  };

  Game();

	int musicID;
	int jumpSfx;
	int stompSfx;
  int successSfx;

	olc::Sprite* testSpr{ nullptr };
	olc::Decal* testDec{ nullptr };

  olc::Renderable plWalk;
  olc::Renderable plIdle;
	olc::Renderable plJump;
	olc::Renderable enZombie;
	olc::Renderable tiles;

	std::array<std::int8_t, (std::int8_t)globals::layersNum> layers;

  EntityId lvlEnt{};
  EntityId player{};

public:
  bool initialize();

  EntityId createZombie(olc::vf2d pos, bool fall_off_cliff, float patrolVelx = -10.0f);

  bool OnUserCreate() override;

  bool OnUserDestroy() override;

  bool OnUserUpdate(float fElapsedTime) override;
};

#endif