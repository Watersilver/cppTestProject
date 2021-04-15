#ifndef ECSOBJ_H
#define ECSOBJ_H

#include "Ecs.h"
#include "components.h"

using namespace components;

namespace ecs
{
	// Register all components
	using Type = Ecs<
		Position,
		Velocity,
		Acceleration,
		Scale,
		Angle,
		Player,
    Visible,
		Controllable,
    Camera,
    Tiles16,
    BoundingBox,
    CollidesWithStatic,
    CollidesWithBoundingBox,
    Grounded,
    Gravity,
    GravityFactor,
    PrevPosition,
    ControlParameters,
    ClampedAccel,
    Animation,
    DeathCounter,
    Zombie,
    Killed,
    Checkpoint,
    Patrol
	>;

	using EntityId = Type::EntityId;
	using Signature = Type::Signature;
	using System = Type::System;

	extern Type world;
}

#endif