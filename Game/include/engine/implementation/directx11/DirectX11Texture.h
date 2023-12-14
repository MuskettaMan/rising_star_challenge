
#pragma once

#include "Engine/ITexture.h"
#include <d3d11.h>

class DirectX11Texture : public ITexture
{
public:

	DirectX11Texture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler, const D3D11_TEXTURE2D_DESC& description);
	virtual ~DirectX11Texture();

	virtual void Update();
	virtual float GetWidth() const;
	virtual float GetHeight() const;

private:

	ID3D11DeviceContext* _context;
	ID3D11ShaderResourceView* _texture;
	ID3D11SamplerState* _sampler;
	D3D11_TEXTURE2D_DESC _description;
};

