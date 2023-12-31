#pragma once

struct Camera;
struct CameraMatrix;

class ECS
{
public:
	static ECS& Default();

	entt::registry& Registry() { return _registry; }
	entt::entity CreateEntity();
	entt::entity CreateGameObject(const std::string& name);
	Camera& GetCamera();
	CameraMatrix& GetCameraMatrix();

private:
	ECS() = default;
	~ECS() = default;
	entt::registry _registry;

	uuids::uuid GenerateUUID();
};
