#pragma once
#include <engine/ecs.hpp>
#include "engine/sprite_animation.hpp"
#include "visit_struct/visit_struct.hpp"
#include <engine/direction.hpp>
#include "engine/ITime.hpp"

class IInput;
class PhysicsWorld;
class IGraphics;

struct CharacterController
{
	float speed{ 1.0f };
	bool isMoving{ false };
	Direction direction{ Direction::Up };

	CharacterController(float speed) : speed(speed) {};
};

struct CharacterAnimations
{
	ResourceHandle<Spritesheet> idleSheet;
	ResourceHandle<Spritesheet> moveSheet;
};

void UpdateCharacterControllers(IInput& input, PhysicsWorld& physics, ECS& ecs = ECS::Default());
void UpdateCharacterAnimations(IGraphics& graphics, ITime& time, ECS& ecs = ECS::Default());

VISITABLE_STRUCT(CharacterController, speed, isMoving, direction);
VISITABLE_STRUCT(CharacterAnimations, idleSheet, moveSheet);
