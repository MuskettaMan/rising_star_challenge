#pragma once

class IGraphics;
class BaseTab;

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

	std::vector<std::unique_ptr<BaseTab>> _tabs;
};

