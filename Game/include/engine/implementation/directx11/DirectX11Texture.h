
#pragma once

#include "Engine/ITexture.h"
#include <d3d11.h>

class DirectX11Texture : public ITexture
{
public:

	DirectX11Texture(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11ShaderResourceView> texture, ComPtr<ID3D11SamplerState> sampler, const D3D11_TEXTURE2D_DESC& description);
	virtual ~DirectX11Texture();

	virtual void Update();
	virtual float GetWidth() const;
	virtual float GetHeight() const;

private:

	const ComPtr<ID3D11DeviceContext> _context;
	ComPtr<ID3D11ShaderResourceView> _texture;
	ComPtr<ID3D11SamplerState> _sampler;
	D3D11_TEXTURE2D_DESC _description;
};

