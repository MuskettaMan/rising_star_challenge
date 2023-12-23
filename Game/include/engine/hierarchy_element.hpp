#pragma once

#include <vector>
#include "entt/fwd.hpp"

struct HierarchyElement
{
	entt::entity parent;
	std::vector<entt::entity> children;
};

struct HierarchyRoot
{

};

void AttachEntityToRoot(entt::entity entity);
void AttachEntityToParent(entt::entity child, entt::entity parent);
void RemoveEntityFromHierarchy(entt::entity entity);
