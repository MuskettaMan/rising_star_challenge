#pragma once
#include <directxmath.h>
#include "visit_struct/visit_struct.hpp"
#include "engine/ecs.hpp"

using namespace DirectX;

class IInput;

struct CameraMatrix
{
	XMMATRIX projection;
	XMMATRIX view;
};

struct Camera
{
	float size{ 30.0f };
	float nearPlane{ 0.1f };
	float farPlane{ 20.0f };
};

VISITABLE_STRUCT(Camera, size, nearPlane, farPlane);


XMFLOAT2 ScreenToWorld(entt::entity cameraEntity, XMFLOAT2 screen, IInput& input, ECS& _ecs = ECS::Default());
