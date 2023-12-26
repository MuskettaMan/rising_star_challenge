#pragma once

#include "visit_struct/visit_struct.hpp"

struct BoxCollider
{
	float width;
	float height;

};

VISITABLE_STRUCT(BoxCollider, width, height);
