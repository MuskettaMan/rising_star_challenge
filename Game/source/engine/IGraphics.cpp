#include "pch.h"
#include "engine/IGraphics.h"
#include "engine/ITexture.h"
#include "engine/IShader.h"
#include "engine/IRenderable.h"


IGraphics::IGraphics() : _renderables(), _textures()
{
}

IGraphics::~IGraphics()
{
    _renderables.clear();
    _textures.clear();
}
