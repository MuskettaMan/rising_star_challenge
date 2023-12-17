#include "pch.h"
#include "editor/hierarchy_tab.hpp"
#include <engine/gameobject.hpp>
#include <engine/ecs.hpp>

constexpr ImGuiTreeNodeFlags BASE_NODE_FLAGS = 
	ImGuiTreeNodeFlags_OpenOnArrow | 
	ImGuiTreeNodeFlags_OpenOnDoubleClick | 
	ImGuiTreeNodeFlags_SpanFullWidth | 
	ImGuiTreeNodeFlags_FramePadding;

HierarchyTab::HierarchyTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags) : BaseTab("Hierarchy", dockID, windowFlags), _selectedEntity(selectedEntity)
{
}

void HierarchyTab::DrawContents()
{
	auto view = ECS::Instance().Registry().view<GameObject>();
	for (auto& entity : view)
	{
		GameObject& gameObject = view.get<GameObject>(entity);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 4.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
		
		ImGuiTreeNodeFlags nodeFlags = BASE_NODE_FLAGS;
		if (_selectedEntity == entity)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool nodeOpen = ImGui::TreeNodeBehavior(ImGui::GetID(reinterpret_cast<void*>(entity)), nodeFlags, gameObject.name.c_str(), nullptr);
		if (ImGui::IsItemClicked())
		{
			_selectedEntity = entity;
		}
		if (nodeOpen)
		{
			//DrawRecursive(child, openElementCount);
			ImGui::TreePop();
		}

		ImGui::PopStyleVar(2);


	}
}
