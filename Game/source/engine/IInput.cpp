#include "pch.h"
#include "engine/IInput.h"

IInput::IInput() : _currentGamepadState(), _previousGamepadState(), _currentKeyboardState(), _previousKeyboardState(), _currentMouseState(), _previousMouseState()
{
}

void IInput::Update()
{
    _previousGamepadState = _currentGamepadState;
    _previousKeyboardState = _currentKeyboardState;
    _previousMouseState = _currentMouseState;
}

float IInput::GetValue(GamepadInput action) const
{
    return _currentGamepadState[static_cast<uint32_t>(action)];
}

bool IInput::IsPressed(GamepadInput action, float threshold) const
{
    return abs(_currentGamepadState[static_cast<uint32_t>(action)]) > threshold && abs(_previousGamepadState[static_cast<uint32_t>(action)]) < threshold;
}

bool IInput::IsReleased(GamepadInput action, float threshold) const
{
    return abs(_currentGamepadState[static_cast<uint32_t>(action)]) < threshold && abs(_previousGamepadState[static_cast<uint32_t>(action)]) > threshold;
}

bool IInput::IsHeld(GamepadInput action, float threshold) const
{
    return abs(_currentGamepadState[static_cast<uint32_t>(action)]) > threshold && abs(_previousGamepadState[static_cast<uint32_t>(action)]) > threshold;
}

bool IInput::IsPressed(KeyboardInput action) const
{
    return _currentKeyboardState[static_cast<uint32_t>(action)] && !_previousKeyboardState[static_cast<uint32_t>(action)];
}

bool IInput::IsReleased(KeyboardInput action) const
{
    return !_currentKeyboardState[static_cast<uint32_t>(action)] && _previousKeyboardState[static_cast<uint32_t>(action)];
}

bool IInput::IsHeld(KeyboardInput action) const
{
    return _currentKeyboardState[static_cast<uint32_t>(action)] && _previousKeyboardState[static_cast<uint32_t>(action)];
}

bool IInput::IsPressed(MouseInput action) const
{
    return _currentMouseState[static_cast<uint32_t>(action)] && !_previousMouseState[static_cast<uint32_t>(action)];
}

bool IInput::IsReleased(MouseInput action) const
{
    return !_currentMouseState[static_cast<uint32_t>(action)] && _previousMouseState[static_cast<uint32_t>(action)];
}

bool IInput::IsHeld(MouseInput action) const
{
    return _currentMouseState[static_cast<uint32_t>(action)] && _previousMouseState[static_cast<uint32_t>(action)];
}
