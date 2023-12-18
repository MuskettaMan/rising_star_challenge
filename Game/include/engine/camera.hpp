#pragma once
#include <directxmath.h>
#include "visit_struct/visit_struct.hpp"

using namespace DirectX;

struct CameraMatrix
{
	XMMATRIX projection;
	XMMATRIX view;
};

struct Camera
{
	float size{ 1.0f };
	float nearPlane{ 0.1f };
	float farPlane{ 20.0f };
};

VISITABLE_STRUCT(Camera, size, nearPlane, farPlane);
