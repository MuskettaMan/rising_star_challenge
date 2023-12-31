#pragma once

#include "Engine/IInput.h"

class DirectXInput : public IInput
{
public:

	DirectXInput();
	virtual ~DirectXInput();

	virtual void Update();
	XMFLOAT2 GetMousePosition() const override;
	XMFLOAT2 GetScreenSize() const override;

private:

	float CalculateTriggerValue(BYTE rawValue, float threshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD, float max = 255.0f);
	float CalculateAxisValue(SHORT rawValue, float deadzone, float left = -32768.0f, float right = 32767.0f);

	XINPUT_STATE _state;

};

