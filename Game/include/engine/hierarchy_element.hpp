#pragma once

#include <vector>
#include "entt/fwd.hpp"
#include "engine/ecs.hpp"

struct HierarchyElement
{
	entt::entity parent;
	std::vector<entt::entity> children;
};

struct HierarchyRoot
{

};

void AttachEntityToRoot(entt::entity entity, ECS& ecs = ECS::Default());
void AttachEntityToParent(entt::entity child, entt::entity parent, ECS& ecs = ECS::Default());
void RemoveEntityFromHierarchy(entt::entity entity, ECS& ecs = ECS::Default());
