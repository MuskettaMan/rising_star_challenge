
#pragma once

#include "engine/IApplication.h"

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
	void HorizontalMovement(float value);
	void VerticalMovement(float value);

	ECS& _ecs;
	entt::entity _root;
	std::unique_ptr<PhysicsWorld> _physicsWorld;
	std::unique_ptr<InputHandler> _inputHandler;

	entt::entity _dynamic;
};

