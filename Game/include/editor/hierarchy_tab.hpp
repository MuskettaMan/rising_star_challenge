#pragma once

#include "editor/base_tab.hpp"

class HierarchyTab : public BaseTab
{
public:
	HierarchyTab(ImGuiID dockID, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;
};