#pragma once

#include "editor/base_tab.hpp"

class IGraphics;

class SceneTab : public BaseTab
{
public:
	SceneTab(ImGuiID dockID, IGraphics& graphics, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;

private:
	IGraphics& _graphics;

	void DrawHandle(entt::entity entity);
};
