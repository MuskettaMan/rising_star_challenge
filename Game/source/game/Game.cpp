#include "pch.h"
#include "game/Game.h"

#include "Engine/IGraphics.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"
#include "Engine/IRenderable.h"
#include "Engine/IInput.h"
#include <engine/ecs.hpp>
#include <engine/camera.hpp>
#include <engine/transform.hpp>
#include <engine/sprite_renderer.hpp>
#include "entt/core/hashed_string.hpp"

using namespace entt::literals;

#define CLAMP(v, x, y) fmin(fmax(v, x), y)

constexpr float PI = 3.14159265359f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float DeltaTime = 0.016f;
constexpr float SpinSpeed = 0.1f;
constexpr float WinTolerance = PI / 10.0f;

std::unique_ptr<IApplication> GetApplication(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input)
{
	return std::make_unique<Game>(graphics, input);
}

Game::Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input) : IApplication(graphics, input)
{
}

Game::~Game()
{
}

bool Game::IsValid()
{
	return true;
}

bool Game::Load()
{
	entt::entity cameraEntity = ECS::Instance().CreateGameObject("Camera");
	ECS::Instance().Registry().emplace<Camera>(cameraEntity);
	ECS::Instance().Registry().emplace<CameraMatrix>(cameraEntity);

	_graphics->CreateTexture(L"assets\\textures\\Arrow.dds");
	_graphics->CreateTexture(L"assets\\textures\\InnerRing.dds");


	entt::entity spriteEntity = ECS::Instance().CreateGameObject("Sprite renderer");
	auto& spriteRenderer = ECS::Instance().Registry().emplace<SpriteRenderer>(spriteEntity);
	spriteRenderer.texture = _graphics->CreateTexture(L"assets\\textures\\sprite.dds");
	spriteRenderer.shader = _graphics->CreateShader(L"assets\\shaders\\UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0");
	spriteRenderer.shader = _graphics->CreateShader(L"assets\\shaders\\UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0");
	spriteRenderer.mesh = _graphics->CreateBillboard(512, 512);
	spriteRenderer.mesh = _graphics->CreateBillboard(512, 512);
	spriteRenderer.mesh = _graphics->CreateBillboard(512, 512);
	spriteRenderer.mesh = _graphics->CreateBillboard(512, 512);

	return true;
}

void Game::Update()
{
	UpdateMatrices();
}

void Game::Cleanup()
{

}

void Game::UpdateMatrices()
{
	auto view = ECS::Instance().Registry().view<Transform, TransformMatrix>();
	for (auto entity : view)
	{
		auto [transform, transformMatrix] = view.get(entity);
		XMMATRIX translation = XMMatrixTranslation(transform.position.x, transform.position.y, 0.0f);
		XMMATRIX rotation = XMMatrixRotationZ(transform.rotation);
		XMMATRIX scale = XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f);
		transformMatrix.worldMatrix = scale * rotation * translation;
		transformMatrix.worldToLocalMatrix = XMMatrixIdentity();
	}
}
