#include "pch.h"
#include "engine/gameobject.hpp"

GameObject GameObject::Null()
{
	return GameObject{ std::string{}, uuids::uuid(), entt::null };
}
