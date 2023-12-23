#pragma once

#include "editor/base_tab.hpp"

struct HierarchyElement;

class HierarchyTab : public BaseTab
{
public:
	HierarchyTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;

private:
	entt::entity& _selectedEntity;
	
	void DrawRecursive(const HierarchyElement& element, int& openElementCount);

};