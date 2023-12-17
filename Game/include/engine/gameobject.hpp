#pragma once

struct GameObject
{
	std::string name;
	uuids::uuid uuid;
	entt::entity entity;

	static GameObject Null();
};

inline bool operator== (const GameObject& lhs, const GameObject& rhs)
{
	return lhs.entity == rhs.entity;
}
inline bool operator!= (const GameObject& lhs, const GameObject& rhs)
{
	return !(lhs == rhs);
}
