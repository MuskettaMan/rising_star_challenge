#include "pch.h"
#include "game/game.hpp"

#include "engine/IGraphics.h"
#include "engine/IInput.h"
#include <engine/ecs.hpp>
#include <engine/camera.hpp>
#include <engine/transform.hpp>
#include <engine/sprite_renderer.hpp>
#include "entt/core/hashed_string.hpp"
#include <engine/hierarchy_element.hpp>

#include "engine/box_collider.hpp"
#include "engine/physics_world.hpp"
#include <engine/physics_debug_drawer.hpp>

using namespace entt::literals;

std::unique_ptr<IApplication> GetApplication(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input)
{
	return std::make_unique<Game>(graphics, input);
}

Game::Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input) : IApplication(graphics, input), _physicsWorld(std::make_unique<PhysicsWorld>(std::make_unique<PhysicsDebugDrawer>(*_graphics)))
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
	_root = ECS::Instance().CreateEntity();
	ECS::Instance().Registry().emplace<HierarchyElement>(_root);
	ECS::Instance().Registry().emplace<HierarchyRoot>(_root);
	ECS::Instance().Registry().emplace<TransformMatrix>(_root);

	entt::entity cameraEntity = ECS::Instance().CreateGameObject("Camera");
	ECS::Instance().Registry().patch<Transform>(cameraEntity, [](auto& transform) { transform.position = { 50.0f, -50.0f }; });
	ECS::Instance().Registry().emplace<HierarchyElement>(cameraEntity);
	ECS::Instance().Registry().emplace<Camera>(cameraEntity, 80.0f, 0.1f, 20.0f);
	ECS::Instance().Registry().emplace<CameraMatrix>(cameraEntity);

	AttachEntityToRoot(cameraEntity);


	entt::entity spriteEntity = ECS::Instance().CreateGameObject("Sprite renderer");
	ECS::Instance().Registry().emplace<HierarchyElement>(spriteEntity);
	ECS::Instance().Registry().patch<Transform>(spriteEntity, [](auto& transform) { transform.position = { 30.0f, -65.0f }; transform.scale = { 1.5f, 0.25f }; transform.rotation = -0.35f; });
	ECS::Instance().Registry().emplace<BoxCollider>(spriteEntity, b2_staticBody, 16.0f, 16.0f);
	auto& spriteRenderer = ECS::Instance().Registry().emplace<SpriteRenderer>(spriteEntity);
	spriteRenderer.texture = _graphics->CreateTexture(L"assets\\textures\\sprite.dds");
	spriteRenderer.shader = _graphics->CreateShader(L"assets\\shaders\\Unlit.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0");
	spriteRenderer.mesh = _graphics->CreateBillboard(16.0f, 16.0f);

	entt::entity spriteEntity3 = ECS::Instance().CreateGameObject("Sprite renderer");
	ECS::Instance().Registry().emplace<HierarchyElement>(spriteEntity3);
	ECS::Instance().Registry().patch<Transform>(spriteEntity3, [](auto& transform) { transform.position = { 45.0f, -69.0f }; transform.scale = { 0.5f, 0.25f }; });
	ECS::Instance().Registry().emplace<BoxCollider>(spriteEntity3, b2_staticBody, 16.0f, 16.0f);
	auto& spriteRenderer3 = ECS::Instance().Registry().emplace<SpriteRenderer>(spriteEntity3, spriteRenderer);

	entt::entity spriteEntity2 = ECS::Instance().CreateGameObject("Sprite renderer");
	ECS::Instance().Registry().emplace<HierarchyElement>(spriteEntity2);
	ECS::Instance().Registry().patch<Transform>(spriteEntity2, [](auto& transform) { transform.position = { 20.0f, 0.0f }; transform.scale = { 0.25f, 0.25f }; });
	ECS::Instance().Registry().emplace<BoxCollider>(spriteEntity2, b2_dynamicBody, 16.0f, 16.0f);
	_physicsWorld->GetBody(spriteEntity2).SetLinearVelocity({ 0.0f, -1.0f });
	auto& spriteRenderer2 = ECS::Instance().Registry().emplace<SpriteRenderer>(spriteEntity2, spriteRenderer);

	AttachEntityToRoot(spriteEntity);
	AttachEntityToRoot(spriteEntity2);
	AttachEntityToRoot(spriteEntity3);

	return true;
}

void Game::Update()
{
	_physicsWorld->Update();
	BuildMatrices();
}

void Game::Cleanup()
{

}

void Game::BuildMatrices()
{
	auto rootView = ECS::Instance().Registry().view<const HierarchyRoot, const HierarchyElement, TransformMatrix>();
	auto [rootHierarchyElement, rootTransformMatrix] = rootView.get<const HierarchyElement, TransformMatrix>(*rootView.begin());

	rootTransformMatrix.worldMatrix = XMMatrixIdentity();
	rootTransformMatrix.worldToLocalMatrix = XMMatrixIdentity();

	BuildChildMatrices(rootTransformMatrix, rootHierarchyElement);
}

void Game::BuildChildMatrices(const TransformMatrix& parentMatrix, const HierarchyElement& parent)
{
	for (int i = 0; i < parent.children.size(); ++i)
	{
		entt::entity child = parent.children[i];

		const auto& childHierarchyElement = ECS::Instance().Registry().get<HierarchyElement>(child);
		const auto& childTransform = ECS::Instance().Registry().get<Transform>(child);
		auto& childTransformMatrix = ECS::Instance().Registry().get<TransformMatrix>(child);

		childTransformMatrix.localMatrix = XMMatrixIdentity();
		XMMATRIX translation = XMMatrixTranslation(childTransform.position.x, childTransform.position.y, 0.0f);
		XMMATRIX rotation = XMMatrixRotationZ(childTransform.rotation);
		XMMATRIX scale = XMMatrixScaling(childTransform.scale.x, childTransform.scale.y, 1.0f);
		childTransformMatrix.localMatrix = scale * rotation * translation;

		childTransformMatrix.worldMatrix = childTransformMatrix.localMatrix * parentMatrix.worldMatrix;

		XMVECTOR determinant = XMMatrixDeterminant(parentMatrix.worldMatrix);
		childTransformMatrix.worldToLocalMatrix = XMMatrixInverse(&determinant, parentMatrix.worldMatrix);

		BuildChildMatrices(childTransformMatrix, childHierarchyElement);
	}
}
