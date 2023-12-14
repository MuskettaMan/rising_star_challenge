
#pragma once

#include "Engine/IApplication.h"

class IGraphics;
class ITexture;
class IShader;
class IRenderable;

enum RingLayer { Outer, Middle, Inner, NumRings };
enum GameState { Setup, Playing, Test, NumModes };
static const unsigned int NumberOfRings = static_cast<int>(NumRings);

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

	void SetupEachRing();
	void UpdateRingSelection();
	void UpdateSelectedRingRotation();
	void UpdateRingTestSelection();
	void TestRingSolution();

	std::shared_ptr<IRenderable> _rings[NumberOfRings];
	std::shared_ptr<IRenderable> _arrow;
	RingLayer _selectedRing;
	GameState _state;
};

