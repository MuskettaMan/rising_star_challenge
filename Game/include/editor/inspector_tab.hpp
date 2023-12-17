#pragma once

#include "editor/base_tab.hpp"

class InspectorTab : public BaseTab
{
public:
	InspectorTab(ImGuiID dockID, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;
};