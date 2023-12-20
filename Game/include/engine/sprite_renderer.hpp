#pragma once
#include "engine/texture.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "engine/resource_handle.hpp"
#include "visit_struct/visit_struct.hpp"

struct SpriteRenderer
{
	ResourceHandle<Texture> texture;
	ResourceHandle<Mesh> mesh;
	ResourceHandle<Shader> shader;
};


VISITABLE_STRUCT(SpriteRenderer, texture, mesh, shader);

