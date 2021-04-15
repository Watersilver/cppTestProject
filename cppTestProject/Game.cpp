#include "Game.h"

#include <array>
#include <vector>
#include <iostream>
#include <cstdint>

#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"
#include "olcPGEX_TransformedView.h"

#include "globals.h"

#include "ecsObj.h"
#include "components.h"
using namespace ecs;
using namespace components;

Game::Game()
{
  // Name your application
  sAppName = "Test Proj";

  // Register systems. They will run in
  // the order they were registered
  world.registerSystem<SysDeath>();
  world.registerSystem<SysAngveltoAngle>();
  world.registerSystem<SysCollideWithZombie>();
  world.registerSystem<SysPrevPos>();
  world.registerSystem<SysClampedA>();
  world.registerSystem<SysAtoV>();
  world.registerSystem<SysApplyGrav>();
  world.registerSystem<SysDetectStaticColls>();
  world.registerSystem<SysResolveStaticColls>();
  world.registerSystem<SysVtoP>();
  world.registerSystem<SysPatrol>();
  world.registerSystem<SysPlControl>();
  world.registerSystem<SysKilled>();
  world.registerSystem<SysCamera>();
  world.registerSystem<SysTileprint>();
  world.registerSystem<SysDrawZombie>();
  world.registerSystem<SysDrawPlayer>();
}

EntityId Game::createZombie(olc::vf2d pos, bool fall_off_cliff, float patrolVelx /*= -10.0*/)
{
  auto zomTest = world.createEntity();
  world.addComponent<Visible>(zomTest);
  world.addComponent<Animation>(zomTest);
  world.addComponent<Angle>(zomTest);
  world.addComponent<Zombie>(zomTest);
  world.addComponent<PrevPosition>(zomTest);
  world.addComponent<Position>(zomTest)->p = pos;
  world.addComponent<BoundingBox>(zomTest);
  world.addComponent<CollidesWithStatic>(zomTest);
  world.addComponent<GravityFactor>(zomTest);
  world.addComponent<Velocity>(zomTest);
  auto p = world.addComponent<Patrol>(zomTest);
  p->speed = patrolVelx;
  p->walk_off = fall_off_cliff;
  world.addComponent<Grounded>(zomTest);
  return zomTest;
}

bool Game::initialize()
{
  // Delete all zombies
  const auto zomPool = world.getComponentPool<Zombie>();
  for (auto& entry : zomPool)
  {
    world.addComponent<DeathCounter>(entry.first)->c = 0.0f;
  }

  // Create player entity
  player = world.createEntity();
  auto ent1pos = world.addComponent<Position>(player);

  // Appear at checkpoint
  auto ch = world.getComponentPool<Checkpoint>();
  if (ch.size() != 1) return false;
  ent1pos->p = world.getComponent<Checkpoint>((*(ch.begin())).first)->p;

  world.addComponent<PrevPosition>(player);
  world.addComponent<Visible>(player);
  world.addComponent<Animation>(player);
  world.addComponent<Controllable>(player);
  world.addComponent<ControlParameters>(player);
  world.addComponent<Velocity>(player);
  world.addComponent<ClampedAccel>(player);
  world.addComponent<Acceleration>(player);
  world.addComponent<Grounded>(player);
  world.addComponent<BoundingBox>(player);
  world.addComponent<Player>(player);
  world.addComponent<Angle>(player);
  world.addComponent<GravityFactor>(player);
  world.addComponent<Scale>(player);
  world.addComponent<CollidesWithStatic>(player);
  world.addComponent<CollidesWithBoundingBox>(player);

  auto cam = world.getComponent<Camera>(lvlEnt);
  if (!cam) return false;
  cam->target.connect(&world.entityConnections);
  cam->target.setId(player);

  // Create all zombies
  createZombie({ 114.000000f, 146.199997f }, false);
  createZombie({ 289.443939f, 196.000000f }, true);
  createZombie({ 375.052948, 163.000000 }, false);
  createZombie({ 597.217712, 113.199997 }, true);
  createZombie({ 831.795959, 114.199997 }, false);
  createZombie({ 1281.817993, 228.000000 }, false);
  createZombie({ 1522.187378, 96.199997 }, false);
  createZombie({ 1829.340454, 126.199997 }, true);
  createZombie({ 2330.641846, 223.000000 }, false);
  createZombie({ 2434.730957, 225.000000 }, false);
  createZombie({ 2837.456299, 207.000000 }, false);
  createZombie({ 3160.791992, 147.199997 }, false);
  createZombie({ 3079.421631, 147.199997 }, false);
  createZombie({ 3013.103027, 148.199997 }, false);
  createZombie({ 3668.096680, 182.000000 }, false);
  createZombie({ 3974.690430, 132.000000 }, false);

  return true;
}

bool Game::OnUserCreate()
{
  // Called once at the start, so create things here
  olc::SOUND::InitialiseAudio();
  //olc::SOUND::SetUserSynthFunction(SoundOut);

  // Load sound stuff
  musicID = olc::SOUND::LoadAudioSample("./assets/sfx/music.wav");
  jumpSfx = olc::SOUND::LoadAudioSample("./assets/sfx/jump.wav");
  stompSfx = olc::SOUND::LoadAudioSample("./assets/sfx/stomp.wav");
  successSfx = olc::SOUND::LoadAudioSample("./assets/sfx/success.wav");

  olc::SOUND::PlaySample(musicID, true);

  tv.Initialise({ ScreenWidth(), ScreenHeight() });

  // Test tv
  tv.SetWorldOffset(olc::vf2d{ -20.0f, -10.0f });
  //tv.SetWorldScale(olc::vf2d{ 1.2, 0.7 });
  // Camera that follows
  //g.tv.SetWorldOffset(pos->p - g.tv.ScaleToWorld(olc::vf2d(g.ScreenWidth() / 2, g.ScreenHeight() / 2)));

  testSpr = new olc::Sprite("./assets/gfx/test.png");
  testDec = new olc::Decal(testSpr);

  plWalk.Load("./assets/gfx/plWalk.png");
  plIdle.Load("./assets/gfx/plIdle.png");
  plJump.Load("./assets/gfx/plJump.png");
  enZombie.Load("./assets/gfx/enZombie.png");
  tiles.Load("./assets/gfx/tiles.png");

  // Initialize all layers to blank and enable them.
  // I think ten will be enough.
  for (auto& layer : layers)
  {
    layer = CreateLayer();
    SetDrawTarget(layer);
    Clear(olc::BLANK);
    EnableLayer(layer, true);
  }

  //// Draw static stuff here to avoid
  //// needless redraws every frame
  SetDrawTarget(layers[globals::backgroundLayer]);
  Clear(olc::VERY_DARK_GREY);
  SetDrawTarget(nullptr);

  // Create level
  lvlEnt = world.createEntity();
  if (!world.addComponent<Visible>(lvlEnt)) return false;
  if (!world.addComponent<Gravity>(lvlEnt)) return false;
  if (!world.addComponent<Checkpoint>(lvlEnt)) return false;
  auto lvlanim = world.addComponent<Animation>(lvlEnt);
  if (!lvlanim) return false;
  lvlanim->speed = 2.0f;
  auto lvlGeom = world.addComponent<Tiles16>(lvlEnt);
  if (!lvlGeom) return false;
  // Create geometry
  lvlGeom->tiles.push_back("............................................................#wwwwwwwww###ww#...#................................................................................................................................................................................#ww#.......");
  lvlGeom->tiles.push_back(".........................................12..............@..#vvvvv##vvvvvvv#######................................12............................................................................................................................................#vv#.......");
  lvlGeom->tiles.push_back(".12.......................12............................@@.##vvvv#..vvvvvvv.....#..12...[]..^..^^^......................................................12.................................12...................................................................1vv........");
  lvlGeom->tiles.push_back("....@@@......................................12........[=]...v#vv...@vvvvvv...........@@############oooooooo...........................12...........................................................................................................@............vv........");
  lvlGeom->tiles.push_back("..............................12.....@..........^.[=]...|....v.v#..[=]vv#vv@..@..............###..##..................................................................12........................................................12................@..............vv........");
  lvlGeom->tiles.push_back("...^.........12.....12.......@.....^@@@^.......^#..|....|....v.v##..|.vv.vv.....@@...........##...##..............@..........12.......................................................@....................12...................................@.12.............vv........");
  lvlGeom->tiles.push_back("########^^^............^...@....#########ooooo###..|wwww@@@..v.v....|.vv.vv.........@@..#....###.###...........[===]...............................................................@.....@@@@@ooooo@@@@@......................................@.........@........vv..@.@.@.");
  lvlGeom->tiles.push_back("###.#######....12......#....@..^#########.....###..|vvv@|@^@.v.v....|.vv.vv...........12.#...###@###.............|......................................12...........................@....@@@.......@@................[========]............@.................12.vv..@@@@@.");
  lvlGeom->tiles.push_back("#######.###......^......^.....#####@#####wwwww###..|vvvv@.#..v.v....|.vv.vv....12...........########.....12......|.....^^^^^^^^^..............12........................12..@.........@.....@@.....@@........^...###.......|..............@......................vv12.@@@..");
  lvlGeom->tiles.push_back("####.########.#ww##...####.#############vvvvvvv##..|vvvv#@#..v.v....|.vv.vv...............#..#######.............|.....[=======].............................12...........@.@.@.....12...@..@.......@.......###..###.......|......#######...................^^^^^vv^^^@@@^^");
  lvlGeom->tiles.push_back("#############.#vv##....##..#..##########vvvvvvv##..|vvvv|..12v.v....|.vv.vv.............#....#######wwwwwwwwwwwww|.......|.|.|...........^............^^............^..@..@.@.@..@.....@....@.......@...12..###...|........|wwwwww#.....#................12.###############");
  lvlGeom->tiles.push_back("##..##.#.####.#vv##............#########vvvvvvv#...|vvvv|....v.v..12|.vv.[==].[==]..[=]......#######vvvvvvvvvvvvv|.......|.|.|.....[]...####..#####...####...#...#####.@..@.@.@......@......@.......@.......###...|........|vvvvvv#.12..#...................###############");
  auto lvlPos = world.addComponent<Position>(lvlEnt);
  if (!lvlPos) return false;
  lvlPos->p.x = 0;
  lvlPos->p.y = 16.0 * 4.0;
  world.addComponent<Camera>(lvlEnt);

  return initialize();
}

bool Game::OnUserDestroy()
{
  olc::SOUND::DestroyAudio();
  return true;
}

bool Game::OnUserUpdate(float fElapsedTime)
{
  if (GetKey(olc::ESCAPE).bPressed)
    return false;

  Clear(olc::BLANK);

  if (fElapsedTime > 0.01f)
    fElapsedTime = 0.01f;

  world.runSystems(fElapsedTime);

  return true;
}
