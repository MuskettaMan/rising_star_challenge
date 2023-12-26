#include "pch.h"
#include "engine/physics_world.hpp"

#include "engine/box_collider.hpp"
#include "engine/ecs.hpp"
#include <engine/transform.hpp>

PhysicsWorld::PhysicsWorld(std::unique_ptr<b2Draw> debugDrawer) : _world(b2Vec2{ 0.0f, -10.0f }), _debugDrawer(std::move(debugDrawer)), _bodies()
{
	_debugDrawer->SetFlags(b2Draw::e_shapeBit);
	_world.SetDebugDraw(_debugDrawer.get());

	ECS::Instance().Registry().on_construct<BoxCollider>().connect<&PhysicsWorld::OnBoxColliderCreate>(*this);
	ECS::Instance().Registry().on_update<BoxCollider>().connect<&PhysicsWorld::OnBoxColliderUpdate>(*this);
	ECS::Instance().Registry().on_destroy<BoxCollider>().connect<&PhysicsWorld::OnBoxColliderDestroy>(*this);
}

void PhysicsWorld::Update()
{
	auto view = ECS::Instance().Registry().view<BoxCollider, Transform>();
	for (auto entity : view)
	{
		auto [collider, transform] = view.get(entity);
		b2Body* body = _bodies[entity];
		b2BodyType type = body->GetType();
		if(type == b2_staticBody)
		{
			body->SetTransform({ transform.position.x, transform.position.y }, transform.rotation);
		}
		else if(type == b2_dynamicBody)
		{
			transform.position.x = body->GetTransform().p.x;
			transform.position.y = body->GetTransform().p.y;
			transform.rotation = body->GetTransform().q.GetAngle();
		}
	}

	constexpr int32_t VELOCITY_ITERATIONS = 6;
	constexpr int32_t POSITION_ITERATIONS = 2;
	_world.Step(1.0f / 60.0f, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	_world.DebugDraw();
}

void PhysicsWorld::OnBoxColliderCreate(entt::registry& registry, entt::entity entity)
{
	auto& boxCollider = registry.get<BoxCollider>(entity);
	auto& transform = registry.get<Transform>(entity);
	b2BodyDef def;
	def.type = boxCollider.type;
	def.position = { transform.position.x, transform.position.y };
	def.angle = transform.rotation;
	auto body = _world.CreateBody(&def);
	_bodies.emplace(entity, body);
	b2PolygonShape shape;
	shape.SetAsBox(boxCollider.width * transform.scale.x, boxCollider.height * transform.scale.y);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	body->CreateFixture(&fixtureDef);
}

void PhysicsWorld::OnBoxColliderUpdate(entt::registry& registry, entt::entity entity)
{
	auto& result = registry.get<BoxCollider>(entity);
	OnBoxColliderDestroy(registry, entity);
	OnBoxColliderCreate(registry, entity);
}

void PhysicsWorld::OnBoxColliderDestroy(entt::registry& registry, entt::entity entity)
{
	_world.DestroyBody(_bodies[entity]);
	_bodies.erase(entity);
}
