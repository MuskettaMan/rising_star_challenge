#include "pch.h"
#include "engine/physics_world.hpp"

#include "engine/box_collider.hpp"
#include "engine/ecs.hpp"

PhysicsWorld::PhysicsWorld(std::unique_ptr<b2Draw> debugDrawer) : _world(b2Vec2{ 0.0f, -10.0f }), _debugDrawer(std::move(debugDrawer)), _bodies()
{
	_debugDrawer->SetFlags(b2Draw::e_shapeBit);
	_world.SetDebugDraw(_debugDrawer.get());

	ECS::Instance().Registry().on_construct<BoxCollider>().connect<&PhysicsWorld::OnBodyDefCreate>(*this);
	ECS::Instance().Registry().on_update<BoxCollider>().connect<&PhysicsWorld::OnBodyDefUpdate>(*this);
	ECS::Instance().Registry().on_destroy<BoxCollider>().connect<&PhysicsWorld::OnBodyDefDestroy>(*this);
}

void PhysicsWorld::Update()
{
	constexpr int32_t VELOCITY_ITERATIONS = 6;
	constexpr int32_t POSITION_ITERATIONS = 2;
	_world.Step(1.0f / 60.0f, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	_world.DebugDraw();
}

void PhysicsWorld::OnBodyDefCreate(entt::registry& registry, entt::entity entity)
{
	auto& result = registry.get<BoxCollider>(entity);
	b2BodyDef def;
	def.type = b2_staticBody;
	auto body = _world.CreateBody(&def);
	_bodies.emplace(entity, body);
	b2PolygonShape shape;
	shape.SetAsBox(result.width, result.height);
	body->CreateFixture(&shape, 0.0f);
}

void PhysicsWorld::OnBodyDefUpdate(entt::registry& registry, entt::entity entity)
{
	auto& result = registry.get<BoxCollider>(entity);
	
	b2PolygonShape* shape = reinterpret_cast<b2PolygonShape*>(_bodies[entity]->GetFixtureList()->GetShape());
	shape->SetAsBox(result.width, result.height);
}

void PhysicsWorld::OnBodyDefDestroy(entt::registry& registry, entt::entity entity)
{
	_world.DestroyBody(_bodies[entity]);
	_bodies.erase(entity);
}
