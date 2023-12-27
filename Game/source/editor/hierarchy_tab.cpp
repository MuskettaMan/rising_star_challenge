#include "pch.h"
#include "editor/hierarchy_tab.hpp"
#include <engine/gameobject.hpp>
#include <engine/ecs.hpp>
#include <engine/hierarchy_element.hpp>

constexpr ImGuiTreeNodeFlags BASE_NODE_FLAGS = 
	ImGuiTreeNodeFlags_OpenOnArrow | 
	ImGuiTreeNodeFlags_OpenOnDoubleClick | 
	ImGuiTreeNodeFlags_SpanFullWidth | 
	ImGuiTreeNodeFlags_FramePadding;

HierarchyTab::HierarchyTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags, ECS& ecs) : BaseTab("Hierarchy", dockID, windowFlags), _selectedEntity(selectedEntity), _ecs(ecs)
{
}

void HierarchyTab::DrawContents()
{
	int openElementCount{ 0 };
	auto rootView = _ecs.Registry().view<HierarchyElement, HierarchyRoot>();
	assert(rootView.begin() != rootView.end() && "No root element found in registry!");
	auto [rootElement] {rootView.get(*rootView.begin()) };

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 4.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
	DrawRecursive(rootElement, openElementCount);
	ImGui::PopStyleVar(2);

    // Available space in the hierarchy window after all the elements have been drawn.
    ImVec2 availableSpace{ ImGui::GetContentRegionAvail() };

    // Creates empty space at the bottom of the hierarchy tab.
    ImGui::Dummy(ImVec2{ availableSpace.x, availableSpace.y });

    // Drag and drop container for root.
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE");

        if (payload)
        {
            entt::entity payloadEntity = *static_cast<entt::entity*>(payload->Data);

            AttachEntityToRoot(payloadEntity);
        }

        ImGui::EndDragDropTarget();
    }
}

void HierarchyTab::DrawRecursive(const HierarchyElement& element, int& openElementCount)
{
    openElementCount += static_cast<int>(element.children.size());

    for (size_t i = 0; i < element.children.size(); ++i)
    {
        const entt::entity childEntity = element.children[i];

        const HierarchyElement& child = _ecs.Registry().get<const HierarchyElement>(childEntity);
        const GameObject& gameObject = _ecs.Registry().get<const GameObject>(childEntity);
        ImGuiTreeNodeFlags nodeFlags = BASE_NODE_FLAGS;

        if (_selectedEntity == childEntity)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        if (child.children.empty())
        {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool nodeOpen = ImGui::TreeNodeBehavior(ImGui::GetID(reinterpret_cast<void*>(childEntity)), nodeFlags, gameObject.name.c_str(), nullptr);
        if (ImGui::IsItemClicked())
        {
            _selectedEntity = childEntity;
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("_TREENODE", &childEntity, sizeof(entt::entity));
            char buffer[128] = "Moving ";
            strcat(buffer, gameObject.name.c_str());
            ImGui::Text(buffer);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE");

            if (payload)
            {
                entt::entity payloadEntity = *static_cast<entt::entity*>(payload->Data);

                AttachEntityToParent(payloadEntity, childEntity);
            }

            ImGui::EndDragDropTarget();
        }

        if (nodeOpen)
        {
            DrawRecursive(child, openElementCount);
            ImGui::TreePop();
        }
    }
}
