
#pragma once

class IGraphics;
class IInput;

class IApplication
{
public:

	IApplication(std::shared_ptr<IGraphics> Graphics, std::shared_ptr<IInput> Input);
	virtual ~IApplication();

	virtual bool IsValid() = 0;
	virtual bool Load() = 0;
	virtual void Update() = 0;
	virtual void Cleanup() = 0;

protected:

	std::shared_ptr<IGraphics> _graphics;
	std::shared_ptr<IInput> _input;

};

