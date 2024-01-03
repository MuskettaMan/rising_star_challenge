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
#include "engine/input_handler.hpp"
#include "engine/box_collider.hpp"
#include "engine/physics_world.hpp"
#include <engine/physics_debug_drawer.hpp>
#include <engine/sprite_animation.hpp>
#include <game/character_controller.hpp>

using namespace entt::literals;

std::unique_ptr<IApplication> GetApplication(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input)
{
	return std::make_unique<Game>(graphics, input);
}

Game::Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input, ECS& ecs) : 
	IApplication(graphics, input), 
	_physicsWorld(std::make_unique<PhysicsWorld>(std::make_unique<PhysicsDebugDrawer>(*_graphics))), 
	_inputHandler(std::make_unique<InputHandler>(*input)),
	_ecs(ecs)
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
	_root = _ecs.CreateEntity();
	_ecs.Registry().emplace<HierarchyElement>(_root);
	_ecs.Registry().emplace<HierarchyRoot>(_root);
	_ecs.Registry().emplace<TransformMatrix>(_root);

	entt::entity cameraEntity = _ecs.CreateGameObject("Camera");
	_ecs.Registry().patch<Transform>(cameraEntity, [](auto& transform) { transform.position = { 50.0f, -50.0f }; });
	_ecs.Registry().emplace<HierarchyElement>(cameraEntity);
	_ecs.Registry().emplace<Camera>(cameraEntity, 80.0f, 0.1f, 20.0f);
	_ecs.Registry().emplace<CameraMatrix>(cameraEntity);

	AttachEntityToRoot(cameraEntity);


	entt::entity spriteEntity = _ecs.CreateGameObject("Sprite renderer");
	_ecs.Registry().emplace<HierarchyElement>(spriteEntity);
	_ecs.Registry().patch<Transform>(spriteEntity, [](auto& transform) { transform.position = { 30.0f, -65.0f }; transform.scale = { 1.5f, 0.25f }; transform.rotation = -0.35f; });
	_ecs.Registry().emplace<BoxCollider>(spriteEntity, b2_staticBody, 16.0f, 16.0f);
	auto& spriteRenderer = _ecs.Registry().emplace<SpriteRenderer>(spriteEntity);
	spriteRenderer.texture = _graphics->CreateTexture(L"assets\\textures\\sprite.dds");
	spriteRenderer.shader = _graphics->CreateShader(L"assets\\shaders\\Unlit.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0");
	spriteRenderer.mesh = _graphics->CreateBillboard(16.0f, 16.0f);

	entt::entity spriteEntity3 = _ecs.CreateGameObject("Sprite renderer");
	_ecs.Registry().emplace<HierarchyElement>(spriteEntity3);
	_ecs.Registry().patch<Transform>(spriteEntity3, [](auto& transform) { transform.position = { 45.0f, -69.0f }; transform.scale = { 0.5f, 0.25f }; });
	_ecs.Registry().emplace<BoxCollider>(spriteEntity3, b2_staticBody, 16.0f, 16.0f);
	auto& spriteRenderer3 = _ecs.Registry().emplace<SpriteRenderer>(spriteEntity3, spriteRenderer);


	_character = CreateCharacter();

	AttachEntityToRoot(spriteEntity);
	AttachEntityToRoot(spriteEntity3);

	
	// Legacy inputs, might be useful later.
	/*AxisInputSetting horizontalSetting{};
	horizontalSetting.gamepad = GamepadInput::LeftStickXAxis;
	horizontalSetting.keyboard = { KeyboardInput::A, KeyboardInput::D };
	_inputHandler->RegisterAxisInput(horizontalSetting, [this](float value) { HorizontalMovement(value); });

	AxisInputSetting verticalSetting{};
	verticalSetting.gamepad = GamepadInput::LeftStickYAxis;
	verticalSetting.keyboard = { KeyboardInput::S, KeyboardInput::W };
	_inputHandler->RegisterAxisInput(verticalSetting, [this](float value) { VerticalMovement(value); });*/

	return true;
}

void Game::Update()
{
	_inputHandler->Update();

	UpdateCharacterControllers(*_input, *_physicsWorld);
	UpdateCharacterAnimations(*_graphics);

	_physicsWorld->Update();
	BuildMatrices();
}

void Game::Cleanup()
{

}

void Game::BuildMatrices()
{
	auto rootView = _ecs.Registry().view<const HierarchyRoot, const HierarchyElement, TransformMatrix>();
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

		const auto& childHierarchyElement = _ecs.Registry().get<HierarchyElement>(child);
		const auto& childTransform = _ecs.Registry().get<Transform>(child);
		auto& childTransformMatrix = _ecs.Registry().get<TransformMatrix>(child);

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

entt::entity Game::CreateCharacter()
{
	entt::entity characterEntity = _ecs.CreateGameObject("Character");
	_ecs.Registry().emplace<HierarchyElement>(characterEntity);
	_ecs.Registry().patch<Transform>(characterEntity, [](auto& transform) { transform.position = { 20.0f, 0.0f }; transform.scale = { 1.0f, 1.0f }; });
	_ecs.Registry().emplace<BoxCollider>(characterEntity, b2_dynamicBody, 8.0f, 8.0f);
	_ecs.Registry().emplace<CharacterController>(characterEntity, 3.0f);

	_textures[0] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\idle_40x40.dds");
	_textures[1] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\death_40x40.dds");
	_textures[2] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\attack_40x40.dds");
	_textures[3] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\run_40x40.dds");

	_sheets[0] = _graphics->CreateSpritesheet(_textures[0], 4, 4);
	_sheets[1] = _graphics->CreateSpritesheet(_textures[1], 9, 4);
	_sheets[2] = _graphics->CreateSpritesheet(_textures[2], 7, 4);
	_sheets[3] = _graphics->CreateSpritesheet(_textures[3], 6, 4);

	auto& characterRenderer = _ecs.Registry().emplace<SpriteRenderer>(characterEntity,
																	  _textures[0], 
																	  _graphics->CreateBillboard(16.0f, 16.0f), 
																	  _graphics->CreateShader(L"assets\\shaders\\Unlit.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0"));

	_ecs.Registry().emplace<SpriteAnimation>(characterEntity, _sheets[0], 1.0f / 4.0f, 0u, 1u);
	_ecs.Registry().emplace<CharacterAnimations>(characterEntity, _sheets[0], _sheets[3]);

	AttachEntityToRoot(characterEntity);

	_physicsWorld->GetBody(characterEntity).SetFixedRotation(true);

	return characterEntity;
}
