#pragma once

#include "editor/base_tab.hpp"
#include "visit_struct/visit_struct.hpp"

inline void InspectValue(const char* name, float& member)
{
    ImGui::InputFloat2(name, &member);
}

inline void InspectValue(const char* name, XMFLOAT2& member)
{
    ImGui::InputFloat2(name, &member.x);
}

inline void InspectValue(const char* name, XMFLOAT3& member)
{
    ImGui::InputFloat3(name, &member.x);
}

struct DrawObject
{
    template <typename T>
    void operator()(const char* name, T& member)
    {
        InspectValue(name, member);
    }

};

class InspectorTab : public BaseTab
{
public:
	InspectorTab(ImGuiID dockID, entt::entity& selectedEntity, ImGuiWindowFlags_ windowFlags = ImGuiWindowFlags_None);

protected:
	virtual void DrawContents() override;

private:
	entt::entity& _selectedEntity;

    template <typename T>
    void InspectObject(T& obj)
    {
        const char* typeName = visit_struct::get_name<T>();
        if (ImGui::CollapsingHeader(typeName, nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
        {
            visit_struct::for_each(obj, DrawObject{});
        }
    }
};