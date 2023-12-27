#pragma once
#include "IInput.h"
#include <array>
#include <unordered_map>
#include <functional>
#include "engine/resource_handle.hpp"

class InputCommand;

struct ButtonInputSetting
{
	GamepadInput gamepad;
	KeyboardInput keyboard;
	MouseInput mouse;
};

struct AxisInputSetting
{
	GamepadInput gamepad;
	std::array<KeyboardInput, 2> keyboard;
};

struct Input {};

class InputHandler
{
public:
	InputHandler(IInput& input);
	ResourceHandle<Input> RegisterAxisInput(AxisInputSetting inputSetting, std::function<void(float)> command);

	void Update();

private:
	IInput& _input;
	std::vector<std::pair<AxisInputSetting, std::function<void(float)>>> _axisInputs;
};