#pragma once

class IGraphics;

class EditorEnvironment
{
public:
	EditorEnvironment(IGraphics& graphics);
	~EditorEnvironment();
	void Update();

private:
	IGraphics& _graphics;
	bool _dockSpaceInitialized{ false };
	ImGuiID _dockRootID;
	ImGuiID _gameID;
	ImGuiID _hierarchyID;
	ImGuiID _inspectorID;
};

