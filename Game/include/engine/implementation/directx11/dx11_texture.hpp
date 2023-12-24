#pragma once

#include "engine/texture.hpp"
#include <d3d11.h>

struct DX11Texture : public Texture
{
	DX11Texture(const ComPtr<ID3D11ShaderResourceView>& _srv, const ComPtr<ID3D11SamplerState>& _sampler, const ComPtr<ID3D11Texture2D>& _texture, const D3D11_TEXTURE2D_DESC& _description)
		: _srv(_srv), _sampler(_sampler), _texture(_texture), _description(_description)
	{
	}

	ComPtr<ID3D11ShaderResourceView> _srv;
	ComPtr<ID3D11SamplerState> _sampler;
	ComPtr<ID3D11Texture2D> _texture;
	D3D11_TEXTURE2D_DESC _description;
};
