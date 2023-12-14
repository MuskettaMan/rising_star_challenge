#include "pch.h"
#include "engine/implementation/directx11/DirectX11Shader.h"
#include "engine/implementation/directx11/DirectX11Texture.h"


DirectX11Shader::DirectX11Shader(ID3D11DeviceContext* context, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout, std::shared_ptr<ITexture> texture) : IShader(texture), _context(context), _vertexShader(vertexShader), _pixelShader(pixelShader), _inputLayout(inputLayout)
{
}

DirectX11Shader::~DirectX11Shader()
{
	if (_inputLayout)
	{
		_inputLayout->Release();
	}

	if (_vertexShader)
	{
		_vertexShader->Release();
	}

	if (_pixelShader)
	{
		_pixelShader->Release();
	}
}

void DirectX11Shader::Update()
{
	if (_context)
	{
		_context->IASetInputLayout(_inputLayout);
		_context->VSSetShader(_vertexShader, 0, 0);
		_context->PSSetShader(_pixelShader, 0, 0);

		if (_texture)
		{
			_texture->Update();
		}
	}
}