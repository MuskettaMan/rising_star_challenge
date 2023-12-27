
#pragma once

enum class GamepadInput : uint32_t
{
	ButtonLeft,
	ButtonRight,
	ButtonTop,
	ButtonBottom,

	DirectionPadLeft,
	DirectionPadRight,
	DirectionPadTop,
	DirectionPadBottom,

	LeftStickXAxis,
	LeftStickYAxis,
	LeftStickPress,

	RightStickXAxis,
	RightStickYAxis,
	RightStickPress,

	TriggerLeft,
	TriggerRight,
	ShoulderButtonLeft,
	ShoulderButtonRight,

	SpecialLeft,
	SpecialRight,

	NumActions,

	DefaultSelect = ButtonBottom,
	DefaultBack = ButtonRight,

};

enum class KeyboardInput : uint32_t
{
	// Alphabet keys
	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	// Number keys
	Num0 = 48, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

	// Arrow keys
	LeftArrow = 37, UpArrow, RightArrow, DownArrow,

	// Modifier keys
	Shift = 16, Ctrl, Alt,

	// Other commonly used keys
	Space = 32, Enter = 13, Escape = 27, Tab = 9, Backspace = 8,

	// Function keys
	F1 = 112, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	NumActions,
};

enum class MouseInput : uint32_t
{
	Left = 0,
	Right,
	Middle,
	X1,
	X2,

	NumActions
};

class IInput
{
public:

	IInput();

	virtual void Update();

	virtual float GetValue(GamepadInput action) const;
	virtual bool IsPressed(GamepadInput action, float threshold = 0.5f) const;
	virtual bool IsReleased(GamepadInput action, float threshold = 0.5f) const;
	virtual bool IsHeld(GamepadInput action, float threshold = 0.5f) const;

	virtual bool IsPressed(KeyboardInput action) const;
	virtual bool IsReleased(KeyboardInput action) const;
	virtual bool IsHeld(KeyboardInput action) const;

	virtual bool IsPressed(MouseInput action) const;
	virtual bool IsReleased(MouseInput action) const;
	virtual bool IsHeld(MouseInput action) const;

	static const uint32_t GamepadNumInputActions = static_cast<uint32_t>(GamepadInput::NumActions);
	static const uint32_t KeyboardNumInputActions = static_cast<uint32_t>(KeyboardInput::NumActions);
	static const uint32_t MouseNumInputActions = static_cast<uint32_t>(MouseInput::NumActions);

protected:
	std::array<float, GamepadNumInputActions> _currentGamepadState;
	std::array<float, GamepadNumInputActions> _previousGamepadState;

	std::array<bool, KeyboardNumInputActions> _currentKeyboardState;
	std::array<bool, KeyboardNumInputActions> _previousKeyboardState;

	std::array<bool, MouseNumInputActions> _currentMouseState;
	std::array<bool, MouseNumInputActions> _previousMouseState;

};

