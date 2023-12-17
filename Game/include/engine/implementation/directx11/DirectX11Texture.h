
#pragma once

#include "Engine/ITexture.h"
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

