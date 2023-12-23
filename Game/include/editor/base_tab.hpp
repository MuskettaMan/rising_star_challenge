#pragma once

class BaseTab
{
public:
	BaseTab(std::string title, ImGuiID dockID, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None, ImVec2 padding = ImVec2{5.0f, 5.0f});
	~BaseTab();
	virtual void Draw();

	ImGuiID GetDockID() const { return _dockID; }

protected:

	virtual void DrawContents() = 0;

	std::string _title;
	ImGuiID _dockID;
	ImGuiWindowFlags_ _flags;
	ImVec2 _padding;
};