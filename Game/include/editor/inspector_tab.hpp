#pragma once

#include "editor/base_tab.hpp"
#include "visit_struct/visit_struct.hpp"
#include <engine/ecs.hpp>

struct Transform;
struct Camera;
struct SpriteRenderer;

inline bool InspectValue(const char* name, float& member)
{
    return ImGui::DragFloat(name, &member, 0.05f);
}

inline bool InspectValue(const char* name, bool& member)
{
    return ImGui::Checkbox(name, &member);
}

inline bool InspectValue(const char* name, b2BodyType& member)
{
    int value{ static_cast<int>(member) };
    bool dirty = ImGui::Combo(name, &value, "Static body\0Kinematic body\0Dynamic body\0");
    member = static_cast<b2BodyType>(value);

    return dirty;
}

inline bool InspectValue(const char* name, XMFLOAT2& member)
{
    return ImGui::DragFloat2(name, &member.x, 0.05f);
}

inline bool InspectValue(const char* name, b2Vec2& member)
{
    return ImGui::DragFloat2(name, &member.x, 0.05f);
}

inline bool InspectValue(const char* name, XMFLOAT3& member)
{
    return ImGui::DragFloat3(name, &member.x, 0.05f);
}

template <typename T>
inline bool InspectValue(const char* name, ResourceHandle<T>& member)
{
    int32_t id{ static_cast<int32_t>(member.Id())};
    return ImGui::DragInt(name, &id, 1);
}

struct DrawObject
{   
    bool dirty = false;

    template <typename T>
    void operator()(const char* name, T& member)
    {
        bool result = InspectValue(name, member);
        if (result)
            dirty = true;
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
        {
            T copy{ *component };
            if(InspectObject(copy))
                ECS::Instance().Registry().replace<T>(entity, copy);
        }
    }

    template <typename T>
    bool InspectObject(T& obj)
    {
        std::string typeName = visit_struct::get_name<T>();
        if (ImGui::CollapsingHeader((GetIcon(typeid(T).hash_code()) + "   " + typeName).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
        {
            DrawObject drawObj{};
            visit_struct::for_each(obj, drawObj);

            return drawObj.dirty;
        }

        return false;
    }
};