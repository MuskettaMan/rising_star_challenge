#pragma once

#include "editor/base_tab.hpp"

class IGraphics;

class SceneTab : public BaseTab
{
public:
	SceneTab(ImGuiID dockID, IGraphics& graphics, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;

private:
	IGraphics& _graphics;
	entt::entity& _selectedEntity;
	
	ImGuizmo::OPERATION _currentGizmo{ ImGuizmo::OPERATION::TRANSLATE };
	ImGuizmo::MODE _mode{ ImGuizmo::MODE::LOCAL };


	void DrawHandle(entt::entity entity);
};
