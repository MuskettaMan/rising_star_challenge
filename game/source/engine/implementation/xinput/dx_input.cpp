#include "pch.h"
#include "engine/implementation/xinput/dx_input.hpp"
#include "editor/scene_tab.hpp"

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
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ButtonLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ButtonRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ButtonTop)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ButtonBottom)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::DirectionPadLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::DirectionPadRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::DirectionPadTop)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::DirectionPadBottom)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::LeftStickPress)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::RightStickPress)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ShoulderButtonLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::ShoulderButtonRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::SpecialLeft)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1.0f : 0.0f;
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::SpecialRight)] = _state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1.0f : 0.0f;

	// Triggers
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::TriggerLeft)] = CalculateTriggerValue(_state.Gamepad.bLeftTrigger);
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::TriggerRight)] = CalculateTriggerValue(_state.Gamepad.bRightTrigger);

	// Stick Axis
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::LeftStickXAxis)] = CalculateAxisValue(_state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::LeftStickYAxis)] = CalculateAxisValue(_state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::RightStickXAxis)] = CalculateAxisValue(_state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	_currentGamepadState[static_cast<uint32_t>(GamepadInput::RightStickYAxis)] = CalculateAxisValue(_state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	for (char key = 'A'; key <= 'Z'; ++key)
		_currentKeyboardState[key] = GetKeyState(key) >> 63;

	for (char key = '0'; key <= '9'; ++key)
		_currentKeyboardState[key] = GetKeyState(key) >> 63;
	
	for (char key = VK_F1; key <= VK_F12; ++key)
		_currentKeyboardState[key] = GetKeyState(key) >> 63;
	
	for (char key = VK_LEFT; key <= VK_DOWN; ++key)
		_currentKeyboardState[key] = GetKeyState(key) >> 63;

	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Shift)] = GetKeyState(VK_SHIFT);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Ctrl)] = GetKeyState(VK_CONTROL);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Alt)] = GetKeyState(VK_MENU);

	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Space)] = GetKeyState(VK_SPACE);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Enter)] = GetKeyState(VK_RETURN);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Escape)] = GetKeyState(VK_ESCAPE);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Tab)] = GetKeyState(VK_TAB);
	_currentKeyboardState[static_cast<uint32_t>(KeyboardInput::Backspace)] = GetKeyState(VK_BACK);

	_currentMouseState[static_cast<uint32_t>(MouseInput::Left)] = GetKeyState(VK_LBUTTON);
	_currentMouseState[static_cast<uint32_t>(MouseInput::Right)] = GetKeyState(VK_RBUTTON);
	_currentMouseState[static_cast<uint32_t>(MouseInput::Middle)] = GetKeyState(VK_MBUTTON);
	_currentMouseState[static_cast<uint32_t>(MouseInput::X1)] = GetKeyState(VK_XBUTTON1);
	_currentMouseState[static_cast<uint32_t>(MouseInput::X2)] = GetKeyState(VK_XBUTTON2);
}

XMFLOAT2 DirectXInput::GetMousePosition() const
{
	ImVec2 offset{ SceneTab::SceneScreenPosition() };
	ImVec2 mousePos{ ImGui::GetIO().MousePos };

	return XMFLOAT2{ mousePos.x - offset.x, mousePos.y - offset.y };
}

XMFLOAT2 DirectXInput::GetScreenSize() const
{
	if (ENABLE_EDITOR)
	{
		return XMFLOAT2{ SceneTab::SceneScreenSize().x, SceneTab::SceneScreenSize().y };
	}
	else
	{
		return XMFLOAT2{ ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y };
	}
}

float DirectXInput::CalculateTriggerValue(BYTE rawValue, float threshold, float max)
{
	return rawValue > threshold ? rawValue / max : 0.0f;
}

float DirectXInput::CalculateAxisValue(SHORT rawValue, float deadzone, float left, float right)
{
	return abs(rawValue) > deadzone ? (-1.0f + (rawValue - left) * (2.0f / (right - left))) : 0.0f;
}

