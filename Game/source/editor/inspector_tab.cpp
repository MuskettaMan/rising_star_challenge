#include "pch.h"
#include "editor/inspector_tab.hpp"
#include <engine/gameobject.hpp>
#include <engine/camera.hpp>
#include <engine/ecs.hpp>
#include "engine/transform.hpp"
#include "visit_struct/visit_struct.hpp"
#include "IconsFontAwesome6.h"
#include <typeinfo>

const std::unordered_map<size_t, std::string> typeToIconMap = {
	{ typeid(Transform).hash_code(), std::string(ICON_FA_UP_DOWN_LEFT_RIGHT) },
	{ typeid(Camera).hash_code(), std::string(ICON_FA_VIDEO) },
};

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

		Transform& transform = ECS::Instance().Registry().get<Transform>(_selectedEntity);
		Camera& camera = ECS::Instance().Registry().get<Camera>(_selectedEntity);
		InspectObject(transform);
		InspectObject(camera);
	}
}

const std::string& InspectorTab::GetIcon(size_t typeID) const
{
	auto it = typeToIconMap.find(typeID);
	if (it == typeToIconMap.end())
		return "";
	
	return it->second;
}
