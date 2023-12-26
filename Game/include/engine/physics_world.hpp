#pragma once

class PhysicsWorld
{
public:
	PhysicsWorld(std::unique_ptr<b2Draw> debugDrawer = nullptr);
	void Update();

	b2World& World() { return _world;  }

private:
	b2World _world;
	std::unique_ptr<b2Draw> _debugDrawer;
	
	std::unordered_map<entt::entity, b2Body*> _bodies;

	void OnBoxColliderCreate(entt::registry& registry, entt::entity entity);
	void OnBoxColliderUpdate(entt::registry& registry, entt::entity entity);
	void OnBoxColliderDestroy(entt::registry& registry, entt::entity entity);
};
