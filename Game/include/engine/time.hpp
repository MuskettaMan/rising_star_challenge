#pragma once
#include "ITime.hpp"

class Time : public ITime
{
public:
	Time();

	std::chrono::duration<float, std::chrono::milliseconds::period> Delta() override;
	std::chrono::duration<double, std::chrono::milliseconds::period> Total() override;

	void Update();

private:
	std::chrono::time_point<std::chrono::system_clock> _start;

	std::chrono::time_point<std::chrono::system_clock> _prevFrame;
	std::chrono::time_point<std::chrono::system_clock> _currFrame;
};