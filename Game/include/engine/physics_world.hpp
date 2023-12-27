#pragma once

class PhysicsWorld
{
public:
	PhysicsWorld(std::unique_ptr<b2Draw> debugDrawer = nullptr, ECS & ecs = ECS::Default());
	void Update();

	b2World& World() { return _world; }
	b2Body& GetBody(entt::entity entity) { return *_bodies[entity]; }

private:
	b2World _world;
	ECS& _ecs;
	std::unique_ptr<b2Draw> _debugDrawer;
	
	std::unordered_map<entt::entity, b2Body*> _bodies;

	void OnBoxColliderCreate(entt::registry& registry, entt::entity entity);
	void OnBoxColliderUpdate(entt::registry& registry, entt::entity entity);
	void OnBoxColliderDestroy(entt::registry& registry, entt::entity entity);
};
