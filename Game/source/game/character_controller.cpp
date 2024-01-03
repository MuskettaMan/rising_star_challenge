#include "pch.h"
#include "game/character_controller.hpp"

#include "engine/box_collider.hpp"
#include "engine/IInput.h"
#include "engine/physics_world.hpp"
#include "engine/IGraphics.h"

void UpdateCharacterControllers(IInput& input, PhysicsWorld& physics, ECS& ecs)
{
	auto view = ecs.Registry().view<CharacterController, const BoxCollider>();

	for (auto entity : view)
	{
		auto [character, collider] = view.get(entity);
		b2Vec2 force{ 0.0f, 0.0f };

		if (input.IsHeld(KeyboardInput::W))
		{
			force.y += 1;
			character.direction = Direction::Up;
		}
		if (input.IsHeld(KeyboardInput::S))
		{
			force.y += -1;
			character.direction = Direction::Down;
		}
		if (input.IsHeld(KeyboardInput::A))
		{
			force.x += -1;
			character.direction = Direction::Left;
		}
		if (input.IsHeld(KeyboardInput::D))
		{
			force.x += 1;
			character.direction = Direction::Right;
		}

		force.Normalize();
		force *= character.speed;

		auto& body = physics.GetBody(entity);
		body.ApplyLinearImpulseToCenter(force, true);

		character.isMoving = body.GetLinearVelocity().LengthSquared() > 0;
	}
}

void UpdateCharacterAnimations(IGraphics& graphics, ECS& ecs)
{
	static float time{ 0.0f };
	time += 0.001666f;

	auto view = ecs.Registry().view<const CharacterAnimations, const CharacterController, SpriteAnimation>();

	for (auto entity : view)
	{
		auto [animations, character, animation] = view.get(entity);

		animation.spritesheet = character.isMoving ? animations.moveSheet : animations.idleSheet;
		animation.currentRow = static_cast<uint32_t>(character.direction);

		uint32_t frame = static_cast<uint32_t>(time / animation.interval);
		animation.currentColumn = frame % graphics.GetSpritesheet(animation.spritesheet).columns;
	}
}
