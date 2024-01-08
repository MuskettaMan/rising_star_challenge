#include "pch.h"
#include "engine/camera.hpp"
#include "engine/transform.hpp"
#include "engine/IInput.h"

XMFLOAT2 ScreenToWorld(entt::entity cameraEntity, XMFLOAT2 screen, IInput& input, ECS& _ecs)
{
	const CameraMatrix& camera = _ecs.Registry().get<const CameraMatrix>(cameraEntity);
	
	XMFLOAT2 normalizedScreenCoords{ screen.x / input.GetScreenSize().x * 2.0f - 1.0f, (input.GetScreenSize().y - screen.y) / input.GetScreenSize().y * 2.0f - 1.0f };

	XMVECTOR detProjection{ XMMatrixDeterminant(camera.projection) };
	XMMATRIX invProjection{ XMMatrixInverse(&detProjection, camera.projection) };

	XMVECTOR clipCoords{ XMVectorSet(normalizedScreenCoords.x, normalizedScreenCoords.y, 0.0f, 1.0f) };

	XMVECTOR eyeCoords{ XMVector4Transform(clipCoords, invProjection) };
	XMVECTOR worldCoords{ eyeCoords.m128_f32[0] / eyeCoords.m128_f32[3], eyeCoords.m128_f32[1] / eyeCoords.m128_f32[3], eyeCoords.m128_f32[2] / eyeCoords.m128_f32[3], 1.0f };

	XMVECTOR detView{ XMMatrixDeterminant(camera.view) };
	XMMATRIX invView{ XMMatrixInverse(&detView, camera.view) };

	XMVECTOR viewCoords{ XMVector4Transform(worldCoords, invView)};

	return XMFLOAT2{ viewCoords.m128_f32[0], viewCoords.m128_f32[1] };
}
