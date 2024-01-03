#pragma once
#include "engine/resource_handle.hpp"
#include "engine/texture.hpp"
#include "visit_struct/visit_struct.hpp"

struct Spritesheet
{
	ResourceHandle<Texture> texture;
	uint32_t columns;
	uint32_t rows;
};

struct SpriteAnimation
{
	ResourceHandle<Spritesheet> spritesheet;

	float interval{ 1.0f / 4.0f };

	uint32_t currentColumn;
	uint32_t currentRow;
};

VISITABLE_STRUCT(SpriteAnimation, spritesheet, interval, currentColumn, currentRow);
