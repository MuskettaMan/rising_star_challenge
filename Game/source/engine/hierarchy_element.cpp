#include "pch.h"
#include "engine/hierarchy_element.hpp"
#include <engine/ecs.hpp>

void AttachEntityToRoot(entt::entity entity)
{
	auto rootView = ECS::Instance().Registry().view<HierarchyRoot>();
	assert(!rootView.empty() && "No root found in registry!");
	assert(ECS::Instance().Registry().try_get<HierarchyElement>(entity) && "Entity passed doesn't contain HierarchyElement component!");

	AttachEntityToParent(entity, rootView[0]);
}

void AttachEntityToParent(entt::entity child, entt::entity parent)
{
	assert(ECS::Instance().Registry().try_get<HierarchyElement>(child) && "Child entity passed doesn't contain HierarchyElement component!");
	assert(ECS::Instance().Registry().try_get<HierarchyElement>(parent) && "Parent entity passed doesn't contain HierarchyElement component!");

	HierarchyElement& childElement{ ECS::Instance().Registry().get<HierarchyElement>(child) };
	HierarchyElement& parentElement{ ECS::Instance().Registry().get<HierarchyElement>(parent) };

	RemoveEntityFromHierarchy(child);

	childElement.parent = parent;
	parentElement.children.push_back(child);
}

void RemoveEntityFromHierarchy(entt::entity entity)
{
	assert(ECS::Instance().Registry().try_get<HierarchyElement>(entity) && "Entity passed doesn't contain HierarchyElement component!");

	HierarchyElement& childElement{ ECS::Instance().Registry().get<HierarchyElement>(entity) };
	if (childElement.parent != entt::null)
	{
		HierarchyElement& parentElement{ ECS::Instance().Registry().get<HierarchyElement>(childElement.parent) };

		auto it = std::find(parentElement.children.begin(), parentElement.children.end(), entity);
		if (it != parentElement.children.end())
		{
			parentElement.children.erase(it);
		}
	}
}