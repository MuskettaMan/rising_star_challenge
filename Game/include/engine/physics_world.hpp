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

	void OnBodyDefCreate(entt::registry& registry, entt::entity entity);
	void OnBodyDefUpdate(entt::registry& registry, entt::entity entity);
	void OnBodyDefDestroy(entt::registry& registry, entt::entity entity);
};
