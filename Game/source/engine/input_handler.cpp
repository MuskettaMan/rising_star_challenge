#include "pch.h"
#include "engine/input_handler.hpp"

InputHandler::InputHandler(IInput& input) : _input(input)
{
}

ResourceHandle<Input> InputHandler::RegisterAxisInput(AxisInputSetting inputSetting, std::function<void(float)> command)
{
	_axisInputs.emplace_back(std::move(inputSetting), std::move(command));
	return ResourceHandle<Input>(_axisInputs.size() - 1);
}

void InputHandler::Update()
{
	for (auto& inputPair : _axisInputs)
	{
		float valueGamepad = _input.GetValue(inputPair.first.gamepad);
		
		bool left = _input.IsHeld(inputPair.first.keyboard[0]);
		bool right = _input.IsHeld(inputPair.first.keyboard[1]);

		float valueKeyboard{static_cast<float>(right - left)};

		float finalValue = valueGamepad + valueKeyboard;
		finalValue = finalValue < -1.0f ? -1.0f : finalValue;
		finalValue = finalValue > 1.0f ? 1.0f : finalValue;

		inputPair.second(finalValue);
	}
}
