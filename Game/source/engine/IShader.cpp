#include "pch.h"
#include "engine/IShader.h"
#include "engine/ITexture.h"

IShader::IShader(std::shared_ptr<ITexture> TextureIn) : _texture(TextureIn)
{
}

const std::shared_ptr<ITexture> IShader::GetTexture() const
{
    return _texture;
}
