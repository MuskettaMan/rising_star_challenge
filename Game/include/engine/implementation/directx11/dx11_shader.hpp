#pragma once

#include "engine/shader.hpp"
#include <d3d11.h>
#include <DirectXMath.h>

struct DX11Shader : public Shader
{
	DX11Shader(const ComPtr<ID3D11VertexShader>& _vertexShader, const ComPtr<ID3D11PixelShader>& _pixelShader, const ComPtr<ID3D11InputLayout>& _inputLayout)
		: _vertexShader(_vertexShader), _pixelShader(_pixelShader), _inputLayout(_inputLayout)
	{
	}

	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11PixelShader> _pixelShader;
	ComPtr<ID3D11InputLayout> _inputLayout;
};
