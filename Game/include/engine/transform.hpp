#pragma once

#include "visit_struct/visit_struct.hpp"

struct Transform
{
	XMFLOAT2 position{ 0.0f, 0.0f };
	float rotation = 0.0f;
	XMFLOAT2 scale{ 1.0f, 1.0f };
};

struct TransformMatrix
{
	XMMATRIX worldMatrix;
	XMMATRIX localMatrix;
	XMMATRIX worldToLocalMatrix;
};

VISITABLE_STRUCT(Transform, position, rotation, scale);
