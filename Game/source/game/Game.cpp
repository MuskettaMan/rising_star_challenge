#include "pch.h"
#include "game/Game.h"

#include "Engine/IGraphics.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"
#include "Engine/IRenderable.h"
#include "Engine/IInput.h"

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

Game::Game(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input) : IApplication(graphics, input), _rings(), _arrow(nullptr), _selectedRing(), _state()
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
	innerTexture = _graphics->CreateTexture(L"resource/textures/InnerRing.dds");
	std::shared_ptr<ITexture> middleTexture = _graphics->CreateTexture(L"resource/textures/MiddleRing.dds");
	std::shared_ptr<ITexture> outerTexture = _graphics->CreateTexture(L"resource/textures/OuterRing.dds");
	std::shared_ptr<ITexture> arrowTexture = _graphics->CreateTexture(L"resource/textures/Arrow.dds");
	std::shared_ptr<IShader> innerShader = _graphics->CreateShader(L"resource/shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", innerTexture);
	std::shared_ptr<IShader> middleShader = _graphics->CreateShader(L"resource/shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", middleTexture);
	std::shared_ptr<IShader> outerShader = _graphics->CreateShader(L"resource/shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", outerTexture);
	std::shared_ptr<IShader> arrowShader = _graphics->CreateShader(L"resource/shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", arrowTexture);
	_rings[static_cast<unsigned int>(RingLayer::Inner)] = _graphics->CreateBillboard(innerShader);
	_rings[static_cast<unsigned int>(RingLayer::Middle)] = _graphics->CreateBillboard(middleShader);
	_rings[static_cast<unsigned int>(RingLayer::Outer)] = _graphics->CreateBillboard(outerShader);
	_arrow = _graphics->CreateBillboard(arrowShader);

	std::srand(static_cast<unsigned int>(std::time(0)));

	_selectedRing = RingLayer::Outer;
	_state = GameState::Setup;

	return true;
}

void Game::Update()
{
	ImGui::Begin("Test");
	//ImGui::Image(_graphics->GetBackBuffer(), ImVec2{300, 300});
	ImGui::End();

	// If mode is Setup game then set each ring to a random rotation
	//if (_state == GameState::Setup)
	//{
	//	SetupEachRing();
	//	_state = GameState::Playing;
	//}

	//// If mode is Playing then read controller input and manage which ring is selected, the rotation of each ring and waiting for select to confirm positions 
	//if (_state == GameState::Playing)
	//{
	//	UpdateRingSelection();
	//	UpdateSelectedRingRotation();
	//	UpdateRingTestSelection();
	//}

	//// If mode is Test then check to see if the rings are in their correct positions, play a noise corresponding to how close the player is 
	//if (_state == GameState::Test)
	//{
	//	TestRingSolution();
	//	_state = GameState::Setup;
	//}
}

void Game::Cleanup()
{

}

void Game::SetupEachRing()
{
	for (unsigned int Ring = 0; Ring < NumberOfRings; ++Ring)
	{
		_rings[Ring]->SetRotation(static_cast<float>(fmod(rand(), PI)));
	}

	_arrow->SetRotation(static_cast<float>(fmod(rand(), PI)));
}

void Game::UpdateRingSelection()
{
	int selectionChange = 0;

	if (_input->IsPressed(InputAction::ShoulderButtonLeft))
	{
		// Change ring selection towards outer
		selectionChange = -1;
	}
	else if (_input->IsPressed(InputAction::ShoulderButtonRight))
	{
		// Change ring selection towards inner
		selectionChange = 1;
	}
		
	_selectedRing = static_cast<RingLayer>(CLAMP(static_cast<int>(_selectedRing) + selectionChange, 0, NumberOfRings - 1));
}

void Game::UpdateSelectedRingRotation()
{
	float delta = _input->GetValue(InputAction::RightStickXAxis) * SpinSpeed * DeltaTime;
	float rotation = _rings[static_cast<int>(_selectedRing)]->GetTransform().rotation;
	float newRotation = static_cast<float>(fmod(rotation + delta, TWO_PI));
	_rings[static_cast<int>(_selectedRing)]->SetRotation(newRotation);
}

void Game::UpdateRingTestSelection()
{
	if (_input->IsPressed(InputAction::DefaultSelect))
	{
		_state = GameState::Test;
	}
}

void Game::TestRingSolution()
{
	float totalRotationDifference = 0.0f;
	float arrowRotation = _arrow->GetTransform().rotation + TWO_PI;

	for (unsigned int Ring = 0; Ring < NumberOfRings; ++Ring)
	{
		totalRotationDifference += abs(arrowRotation - (_rings[Ring]->GetTransform().rotation + TWO_PI));
	}

	float averageRotationDifference = totalRotationDifference / NumberOfRings;

	if (averageRotationDifference < WinTolerance)
	{
		// Win
	}
	else
	{
		// Lose
	}
}
