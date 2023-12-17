#pragma once

class BaseTab
{
public:
	BaseTab(std::string title, ImGuiID dockID, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);
	~BaseTab();
	virtual void Draw();

	ImGuiID GetDockID() const { return _dockID; }

protected:

	virtual void DrawContents() = 0;

	std::string _title;
	ImGuiWindowFlags_ _flags;
	ImGuiID _dockID;
};