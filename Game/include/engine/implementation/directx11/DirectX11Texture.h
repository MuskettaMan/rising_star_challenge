
#pragma once

#include "engine/ITexture.h"
#include "engine/texture.hpp"
#include <d3d11.h>

class DirectX11Texture : public ITexture
{
public:

	DirectX11Texture(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11ShaderResourceView> srv, ComPtr<ID3D11SamplerState> sampler, ComPtr<ID3D11Texture2D> texture, const D3D11_TEXTURE2D_DESC& description);
	virtual ~DirectX11Texture();

	virtual void Update();
	virtual float GetWidth() const;
	virtual float GetHeight() const;
	virtual void* GetTexturePtr() const { return _srv.Get(); }

private:

	const ComPtr<ID3D11DeviceContext> _context;
	ComPtr<ID3D11ShaderResourceView> _srv;
	ComPtr<ID3D11SamplerState> _sampler;
	ComPtr<ID3D11Texture2D> _texture;
	D3D11_TEXTURE2D_DESC _description;
};

struct DirectX11Texture2 : public Texture
{
	DirectX11Texture2(const ComPtr<ID3D11ShaderResourceView>& _srv, const ComPtr<ID3D11SamplerState>& _sampler, const ComPtr<ID3D11Texture2D>& _texture, const D3D11_TEXTURE2D_DESC& _description)
		: _srv(_srv), _sampler(_sampler), _texture(_texture), _description(_description)
	{
	}

	ComPtr<ID3D11ShaderResourceView> _srv;
	ComPtr<ID3D11SamplerState> _sampler;
	ComPtr<ID3D11Texture2D> _texture;
	D3D11_TEXTURE2D_DESC _description;
};
