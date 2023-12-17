#pragma once

class IGraphics;

class Editor
{
public:
	Editor(IGraphics& graphics);
	void Update();

private:
	IGraphics& _graphics;
};