#pragma once

#include "editor/base_tab.hpp"
#include "visit_struct/visit_struct.hpp"
#include <engine/ecs.hpp>

struct Transform;
struct Camera;
struct SpriteRenderer;

inline void InspectValue(const char* name, float& member)
{
    ImGui::DragFloat(name, &member, 0.05);
}

inline void InspectValue(const char* name, XMFLOAT2& member)
{
    ImGui::DragFloat2(name, &member.x, 0.05);
}

inline void InspectValue(const char* name, XMFLOAT3& member)
{
    ImGui::DragFloat3(name, &member.x, 0.05);
}

template <typename T>
inline void InspectValue(const char* name, ResourceHandle<T>& member)
{
    int32_t id{ static_cast<int32_t>(member.Id())};
    ImGui::DragInt(name, &id, 1);
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

    const std::string& GetIcon(size_t typeID) const;

    template <typename T>
    void TryInspect(entt::entity entity)
    {
        T* component = ECS::Instance().Registry().try_get<T>(_selectedEntity);
        if (component)
            InspectObject(*component);
    }

    template <typename T>
    void InspectObject(T& obj)
    {
        std::string typeName = visit_struct::get_name<T>();
        if (ImGui::CollapsingHeader((GetIcon(typeid(T).hash_code()) + "   " + typeName).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
        {
            visit_struct::for_each(obj, DrawObject{});
        }
    }
};