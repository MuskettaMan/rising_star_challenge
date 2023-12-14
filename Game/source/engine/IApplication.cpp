#include "pch.h"
#include "engine/IApplication.h"
#include "engine/IGraphics.h"
#include "engine/IInput.h"

IApplication::IApplication(std::shared_ptr<IGraphics> graphics, std::shared_ptr<IInput> input) : _graphics(graphics), _input(input)
{
}

IApplication::~IApplication()
{
}
