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
	_ecs(ecs),
	_time()
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

	_cameraEntity = _ecs.CreateGameObject("Camera");
	_ecs.Registry().patch<Transform>(_cameraEntity, [](auto& transform) { transform.position = { 50.0f, -50.0f }; });
	_ecs.Registry().emplace<HierarchyElement>(_cameraEntity);
	_ecs.Registry().emplace<Camera>(_cameraEntity, 80.0f, 0.1f, 20.0f);
	_ecs.Registry().emplace<CameraMatrix>(_cameraEntity);

	AttachEntityToRoot(_cameraEntity);


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

	_textures[0] = _graphics->CreateTexture(L"assets\\textures\\hooded_knight\\idle.dds");
	_textures[1] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\death_40x40.dds");
	_textures[2] = _graphics->CreateTexture(L"assets\\textures\\rpg_hero\\attack_40x40.dds");
	_textures[3] = _graphics->CreateTexture(L"assets\\textures\\hooded_knight\\run.dds");
	_textures[4] = _graphics->CreateTexture(L"assets\\textures\\guns.dds");

	_sheets[0] = _graphics->CreateSpritesheet(_textures[0], 3, 4);
	_sheets[1] = _graphics->CreateSpritesheet(_textures[1], 9, 4);
	_sheets[2] = _graphics->CreateSpritesheet(_textures[2], 7, 4);
	_sheets[3] = _graphics->CreateSpritesheet(_textures[3], 10, 4);
	_sheets[4] = _graphics->CreateSpritesheet(_textures[4], 1, 5);

	_character = CreateCharacter();

	_gunPivot = _ecs.CreateGameObject("Gun Pivot");
	_ecs.Registry().emplace<HierarchyElement>(_gunPivot);

	entt::entity gunEntity = _ecs.CreateGameObject("Gun");
	_ecs.Registry().emplace<HierarchyElement>(gunEntity);
	_ecs.Registry().patch<Transform>(gunEntity, [](auto& transform) { transform.scale = { 1.0f * 0.75, 0.25f * 0.75 }; transform.position = { 10.0f, 0.0f }; });
	auto& rendererGun = _ecs.Registry().emplace<SpriteRenderer>(gunEntity, spriteRenderer);
	rendererGun.texture = _textures[4];
	_ecs.Registry().emplace<SpriteAnimation>(gunEntity, _sheets[4], 0.0f, 0u, 0u);

	AttachEntityToRoot(spriteEntity);
	AttachEntityToRoot(spriteEntity3);

	AttachEntityToParent(_gunPivot, _character);
	AttachEntityToParent(gunEntity, _gunPivot);

	
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
	_time.Update();
	std::stringstream ss;
	ss << std::to_string(_time.Delta().count());
	ss << "\n";
	OutputDebugStringA(ss.str().c_str());

	_inputHandler->Update();

	auto& characterTransform = _ecs.Registry().get<Transform>(_character);
	auto& gunPivotTransform = _ecs.Registry().get<Transform>(_gunPivot);

	XMFLOAT2 characterPos = characterTransform.position;
	XMFLOAT2 mousePos = ScreenToWorld(_cameraEntity, _input->GetMousePosition(), *_input);;
	gunPivotTransform.rotation = std::atan2(mousePos.y - characterPos.y, mousePos.x - characterPos.x);

	
	XMVECTOR mousePosVec = XMLoadFloat2(&mousePos);
	XMVECTOR characterPosVec = XMLoadFloat2(&characterPos);
	float dotProduct = DirectX::XMVector2Dot(XMVector2Normalize(mousePosVec - characterPosVec), XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)).m128_f32[0];
	gunPivotTransform.scale.y = dotProduct > 0 ? 1.0f : -1.0f;

	UpdateCharacterControllers(*_input, *_physicsWorld);
	UpdateCharacterAnimations(*_graphics, _time);

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
