#include "pch.h"
#include "editor/base_tab.hpp"

BaseTab::BaseTab(std::string title, ImGuiID dockID, ImGuiWindowFlags_ windowFlags, ImVec2 padding) : _title(title), _dockID(dockID), _flags(windowFlags), _padding(padding)
{

}

BaseTab::~BaseTab() = default;

void BaseTab::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, _padding);
	ImGui::Begin(_title.c_str(), nullptr, _flags);
	ImGui::PopStyleVar(1);
	DrawContents();
	ImGui::End();
}
