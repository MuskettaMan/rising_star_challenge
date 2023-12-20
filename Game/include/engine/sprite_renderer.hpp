#pragma once
#include "engine/texture.hpp"
#include "engine/mesh.hpp"
#include "engine/resource_handle.hpp"


struct SpriteRenderer
{
	ResourceHandle<Texture> texture;
	ResourceHandle<Mesh> mesh;
	ResourceHandle<Shader> shader;
};


