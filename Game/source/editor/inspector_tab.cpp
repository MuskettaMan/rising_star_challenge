#include "pch.h"
#include "editor/inspector_tab.hpp"
#include <engine/gameobject.hpp>
#include <engine/camera.hpp>
#include "engine/transform.hpp"
#include "engine/sprite_renderer.hpp"
#include "visit_struct/visit_struct.hpp"
#include "IconsFontAwesome6.h"
#include <typeinfo>

#include "engine/box_collider.hpp"

const std::unordered_map<size_t, std::string> typeToIconMap = {
	{ typeid(Transform).hash_code(), std::string(ICON_FA_UP_DOWN_LEFT_RIGHT) },
	{ typeid(Camera).hash_code(), std::string(ICON_FA_VIDEO) },
	{ typeid(SpriteRenderer).hash_code(), std::string(ICON_FA_IMAGE) },
};

const std::string defaultIcon{ ICON_FA_PUZZLE_PIECE };

InspectorTab::InspectorTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags, ECS& ecs) : BaseTab("Inspector", dockID, windowFlags), _selectedEntity(selectedEntity), _ecs(ecs)
{
}

void InspectorTab::DrawContents()
{
	if (_selectedEntity != entt::null)
	{
		GameObject& gameObject = _ecs.Registry().get<GameObject>(_selectedEntity);

		char nameBuffer[64];
		std::strcpy(nameBuffer, gameObject.name.c_str());
		if (ImGui::InputText("Name", nameBuffer, std::size(nameBuffer)))
		{
			std::strcpy(gameObject.name.data(), nameBuffer);
		}

		TryInspect<Transform>(_selectedEntity);
		TryInspect<Camera>(_selectedEntity);
		TryInspect<SpriteRenderer>(_selectedEntity);
		TryInspect<BoxCollider>(_selectedEntity);
	}
}

const std::string& InspectorTab::GetIcon(size_t typeID) const
{
	auto it = typeToIconMap.find(typeID);
	if (it == typeToIconMap.end())
		return defaultIcon;
	
	return it->second;
}
