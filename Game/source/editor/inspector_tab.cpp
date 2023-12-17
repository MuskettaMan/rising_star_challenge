#include "pch.h"
#include "editor/inspector_tab.hpp"
#include <engine/gameobject.hpp>

InspectorTab::InspectorTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags) : BaseTab("Inspector", dockID, windowFlags), _selectedEntity(selectedEntity)
{
}

void InspectorTab::DrawContents()
{
	if (_selectedEntity != entt::null)
	{
		GameObject& gameObject = ECS::Instance().Registry().get<GameObject>(_selectedEntity);

		char nameBuffer[64];
		std::strcpy(nameBuffer, gameObject.name.c_str());
		if (ImGui::InputText("Name", nameBuffer, std::size(nameBuffer)))
		{
			std::strcpy(gameObject.name.data(), nameBuffer);
		}
	}
}
