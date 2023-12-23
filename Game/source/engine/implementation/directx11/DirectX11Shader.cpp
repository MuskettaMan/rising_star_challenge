#include "pch.h"
#include "engine/implementation/directx11/DirectX11Shader.h"
#include "engine/implementation/directx11/DirectX11Texture.h"


DirectX11Shader::DirectX11Shader(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11VertexShader> vertexShader, ComPtr<ID3D11PixelShader> pixelShader, ComPtr<ID3D11InputLayout> inputLayout, std::shared_ptr<ITexture> texture) : IShader(texture), _context(context), _vertexShader(vertexShader), _pixelShader(pixelShader), _inputLayout(inputLayout)
{
}

void DirectX11Shader::Update()
{
	if (_context)
	{
		_context->IASetInputLayout(_inputLayout.Get());
		_context->VSSetShader(_vertexShader.Get(), 0, 0);
		_context->PSSetShader(_pixelShader.Get(), 0, 0);

		if (_texture)
		{
			_texture->Update();
		}
	}
}