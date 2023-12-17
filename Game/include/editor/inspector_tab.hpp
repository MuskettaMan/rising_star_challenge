#pragma once

#include "editor/base_tab.hpp"

class InspectorTab : public BaseTab
{
public:
	InspectorTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;

private:
	entt::entity& _selectedEntity;
};