
#pragma once

#include "engine/IApplication.h"

class IGraphics;
class ITexture;
class IShader;
struct TransformMatrix;
struct HierarchyElement;

class Game : public IApplication
{
public:

	Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input);
	virtual ~Game();

	virtual bool IsValid();
	virtual bool Load();
	virtual void Update();
	virtual void Cleanup();

private:
	void BuildMatrices();
	void BuildChildMatrices(const TransformMatrix& parentMatrix, const HierarchyElement& parent);
	entt::entity _root;

};

