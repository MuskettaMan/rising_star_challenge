#pragma once

#include "visit_struct/visit_struct.hpp"

struct BoxCollider
{
	b2BodyType type{ b2_staticBody };
	float width;
	float height;

};

VISITABLE_STRUCT(BoxCollider, type, width, height);
