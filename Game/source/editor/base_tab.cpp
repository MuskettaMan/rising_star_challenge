#include "pch.h"
#include "editor/base_tab.hpp"

BaseTab::BaseTab(std::string title, ImGuiID dockID, ImGuiWindowFlags_ windowFlags) : _title(title), _flags(windowFlags), _dockID(dockID)
{

}

BaseTab::~BaseTab() = default;

void BaseTab::Draw()
{
	ImGui::Begin(_title.c_str(), nullptr, _flags);
	DrawContents();
	ImGui::End();
}
