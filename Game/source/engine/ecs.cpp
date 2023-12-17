#include "pch.h"
#include "engine/ecs.hpp"
#include <engine/gameobject.hpp>

ECS& ECS::Instance()
{
	static ECS* ecs{ new ECS() };
	return *ecs;
}

entt::entity ECS::CreateEntity()
{
	return _registry.create();
}

entt::entity ECS::CreateGameObject(const std::string& name)
{
	entt::entity entity{ CreateEntity() };
	auto& gameObject = _registry.emplace<GameObject>(entity);
	gameObject.name = name;
	gameObject.entity = entity;
	gameObject.uuid = GenerateUUID();

	return entity;
}

uuids::uuid ECS::GenerateUUID()
{
	std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size> {};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	uuids::uuid_random_generator gen{ generator };

	uuids::uuid id = gen();
	
	// TODO: Make sure UUID isn't already present!
	//assert(_gameObjectMap.find(id) == _gameObjectMap.end());

	return id;
}
