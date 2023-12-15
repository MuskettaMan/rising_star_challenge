
#pragma once

#include "Engine/IShader.h"
#include <d3d11.h>
#include <DirectXMath.h>

class DirectX11Graphics;

class DirectX11Shader : public IShader
{
public:

	DirectX11Shader(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11VertexShader> vertexShader, ComPtr<ID3D11PixelShader> pixelShader, ComPtr<ID3D11InputLayout> inputLayout, std::shared_ptr<ITexture> texture);
	virtual void Update();

private:

	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11PixelShader> _pixelShader;
	ComPtr<ID3D11InputLayout> _inputLayout;
};

