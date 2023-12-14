#include "pch.h"
#include "engine/IInput.h"

IInput::IInput() : _currentState(), _previousState()
{
}

void IInput::Update()
{
    _previousState = _currentState;
}

float IInput::GetValue(InputAction action) const
{
    return _currentState[static_cast<uint32_t>(action)];
}

bool IInput::IsPressed(InputAction action, float threshold) const
{
    return abs(_currentState[static_cast<uint32_t>(action)]) > threshold && abs(_previousState[static_cast<uint32_t>(action)]) < threshold;
}

bool IInput::IsReleased(InputAction action, float threshold) const
{
    return abs(_currentState[static_cast<uint32_t>(action)]) < threshold && abs(_previousState[static_cast<uint32_t>(action)]) > threshold;
}

bool IInput::IsHeld(InputAction action, float threshold) const
{
    return abs(_currentState[static_cast<uint32_t>(action)]) > threshold && abs(_previousState[static_cast<uint32_t>(action)]) > threshold;
}
