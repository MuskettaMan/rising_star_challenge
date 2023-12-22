
#pragma once

#include "Engine/IApplication.h"

class IGraphics;
class ITexture;
class IShader;

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
	void UpdateMatrices();

};

