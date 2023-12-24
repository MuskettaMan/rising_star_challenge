#include "pch.h"
#include "engine/implementation/xinput/dx_input.hpp"

DirectXInput::DirectXInput() : IInput(), _state()
{
}

DirectXInput::~DirectXInput()
{
}

void DirectXInput::Update()
{
	IInput::Update();

	unsigned int result = XInputGetState(0, &_state);

	// Buttons
	_currentState[static_cast<uint32_t>(InputAction::ButtonLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::ButtonRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::ButtonTop)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::ButtonBottom)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::DirectionPadLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::DirectionPadRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::DirectionPadTop)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::DirectionPadBottom)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::LeftStickPress)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::RightStickPress)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::ShoulderButtonLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::ShoulderButtonRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::SpecialLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1.0f : 0.0f;
	_currentState[static_cast<uint32_t>(InputAction::SpecialRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1.0f : 0.0f;

	// Triggers
	_currentState[static_cast<uint32_t>(InputAction::TriggerLeft)] = CalculateTriggerValue(_state.Gamepad.bLeftTrigger);
	_currentState[static_cast<uint32_t>(InputAction::TriggerRight)] = CalculateTriggerValue(_state.Gamepad.bRightTrigger);

	// Stick Axis
	_currentState[static_cast<uint32_t>(InputAction::LeftStickXAxis)] = CalculateAxisValue(_state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	_currentState[static_cast<uint32_t>(InputAction::LeftStickYAxis)] = CalculateAxisValue(_state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	_currentState[static_cast<uint32_t>(InputAction::RightStickXAxis)] = CalculateAxisValue(_state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	_currentState[static_cast<uint32_t>(InputAction::RightStickYAxis)] = CalculateAxisValue(_state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float DirectXInput::CalculateTriggerValue(BYTE rawValue, float threshold, float max)
{
	return rawValue > threshold ? rawValue / max : 0.0f;
}

float DirectXInput::CalculateAxisValue(SHORT rawValue, float deadzone, float left, float right)
{
	return abs(rawValue) > deadzone ? (-1.0f + (rawValue - left) * (2.0f / (right - left))) : 0.0f;
}

