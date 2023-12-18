#include "pch.h"
#include "editor/scene_tab.hpp"
#include "engine/IGraphics.h"

SceneTab::SceneTab(ImGuiID dockID, IGraphics& graphics, ImGuiWindowFlags_ windowFlags) : BaseTab("Scene", dockID, windowFlags), _graphics(graphics)
{
}

void SceneTab::DrawContents()
{
	ImGuiIO io{ ImGui::GetIO() };
	ImVec2 displaySize{ io.DisplaySize };
	ImVec2 windowSize{ ImGui::GetWindowSize() };

	float aspectRatioDisplay{ displaySize.x / displaySize.y };
	float aspectRatioWindow{ windowSize.x / windowSize.y };

	ImVec2 newSize{ windowSize };
	ImVec2 dummySpace{ 0.0f, 0.0f };


	if (aspectRatioWindow > aspectRatioDisplay)
	{
		newSize.x = windowSize.y * aspectRatioDisplay;
		dummySpace.x = (windowSize.x - newSize.x) / 2.0f;
		ImGui::Dummy(dummySpace);
		ImGui::SameLine();
	}
	else
	{
		newSize.y = windowSize.x / aspectRatioDisplay;
		dummySpace.y = (windowSize.y - newSize.y) / 2.0f;
		ImGui::Dummy(dummySpace);
	}

	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImVec2 cursorScreenPos{ ImGui::GetCursorScreenPos() };
	ImGuizmo::SetRect(cursorScreenPos.x, cursorScreenPos.y, newSize.x, newSize.y);

	ImGui::Image(_graphics.GetRenderTextureSRV(), newSize);
}

void SceneTab::DrawHandle(entt::entity entity)
{
	// TODO: Require camera matrices and gameobject matrices.
}
