#include "pch.h"
#include "engine/implementation/directx11/DirectX11Texture.h"

DirectX11Texture::DirectX11Texture(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11ShaderResourceView> texture, ComPtr<ID3D11SamplerState> sampler, const D3D11_TEXTURE2D_DESC& description) : _context(context), _texture(texture), _sampler(sampler), _description(description)
{
}

DirectX11Texture::~DirectX11Texture()
{
}

void DirectX11Texture::Update()
{
	if (_context)
	{
		_context->PSSetShaderResources(0, 1, _texture.GetAddressOf());
		_context->PSSetSamplers(0, 1, _sampler.GetAddressOf());
	}
}

float DirectX11Texture::GetWidth() const
{
	return static_cast<float>(_description.Width);
}

float DirectX11Texture::GetHeight() const
{
	return static_cast<float>(_description.Height);
}
