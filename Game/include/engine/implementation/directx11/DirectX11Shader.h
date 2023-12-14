
#pragma once

#include "Engine/IShader.h"
#include <d3d11.h>
#include <DirectXMath.h>

class DirectX11Graphics;

class DirectX11Shader : public IShader
{
public:

	DirectX11Shader(ID3D11DeviceContext* context, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout, std::shared_ptr<ITexture> texture);
	virtual ~DirectX11Shader();
	virtual void Update();

private:

	ID3D11DeviceContext* _context;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
};

