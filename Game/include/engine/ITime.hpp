#pragma once

#include <chrono>

class ITime
{
public:
	virtual ~ITime() = default;

	virtual std::chrono::duration<float, std::chrono::milliseconds::period> Delta() = 0;
	virtual std::chrono::duration<double, std::chrono::milliseconds::period> Total() = 0;
};