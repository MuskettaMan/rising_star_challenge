#include "pch.h"
#include "time.hpp"

using namespace std::literals::chrono_literals;

Time::Time() : _start(std::chrono::system_clock::now()), _prevFrame(_start - 16ms), _currFrame(_start)
{
    
}

std::chrono::duration<float, std::chrono::milliseconds::period> Time::Delta()
{
    return std::chrono::duration<float, std::chrono::milliseconds::period>(_currFrame - _prevFrame);
}

std::chrono::duration<double, std::chrono::milliseconds::period> Time::Total()
{
    return std::chrono::duration<double, std::chrono::milliseconds::period>(std::chrono::system_clock::now() - _start);
}

void Time::Update()
{
    _prevFrame = _currFrame;
    _currFrame = std::chrono::system_clock::now();
}
