#include "pch.h"
#include "engine/ecs.hpp"
#include <engine/gameobject.hpp>
#include "engine/transform.hpp"
#include "engine/hierarchy_element.hpp"
#include <engine/camera.hpp>

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

	auto& transform = _registry.emplace<Transform>(entity);
	transform.position = XMFLOAT2{ 0.0f, 0.0f };
	transform.rotation = 0.0f;
	transform.scale = XMFLOAT2{ 1.0f, 1.0f };

	auto& transformMatrix = _registry.emplace<TransformMatrix>(entity);
	transformMatrix.worldMatrix = XMMatrixIdentity();;

	return entity;
}

Camera& ECS::GetCamera()
{
	auto view = _registry.view<Camera>();
	entt::entity entity = *view.begin();
	return view.get<Camera>(entity);
}

CameraMatrix& ECS::GetCameraMatrix()
{
	auto view = _registry.view<CameraMatrix>();
	entt::entity entity = *view.begin();
	return view.get<CameraMatrix>(entity);
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
