
#pragma once

#include "engine/IApplication.h"
#include "engine/sprite_animation.hpp"
#include "engine/ecs.hpp"
#include "engine/time.hpp"

class PhysicsWorld;
class IGraphics;
class ITexture;
class IShader;
class InputHandler;
struct TransformMatrix;
struct HierarchyElement;

class Game : public IApplication
{
public:

	Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input, ECS& ecs = ECS::Default());
	virtual ~Game();

	virtual bool IsValid();
	virtual bool Load();
	virtual void Update();
	virtual void Cleanup();

private:
	void BuildMatrices();
	void BuildChildMatrices(const TransformMatrix& parentMatrix, const HierarchyElement& parent);

	entt::entity CreateCharacter();

	ECS& _ecs;
	Time _time;
	entt::entity _root;
	std::unique_ptr<PhysicsWorld> _physicsWorld;
	std::unique_ptr<InputHandler> _inputHandler;

	entt::entity _cameraEntity;
	entt::entity _character;
	entt::entity _gunPivot;
	std::array<ResourceHandle<Spritesheet>, 5> _sheets;
	std::array<ResourceHandle<Texture>, 5> _textures;
};

