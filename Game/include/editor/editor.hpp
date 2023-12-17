#pragma once

class IGraphics;
class EditorEnvironment;

class Editor
{
public:
	Editor(IGraphics& graphics);
	~Editor();
	void Update();

private:
	IGraphics& _graphics;
	std::unique_ptr<EditorEnvironment> _editorEnvironment;
};