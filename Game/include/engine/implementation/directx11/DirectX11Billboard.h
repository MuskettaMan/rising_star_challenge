
#pragma once

#include "Engine/IRenderable.h"
#include <d3d11.h>
#include <DirectXMath.h>

class DirectX11Graphics;

class DirectX11Billboard : public IRenderable
{
public:

	DirectX11Billboard(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> vertexBuffer, ComPtr<ID3D11Buffer> indexBuffer, unsigned int vertexStride);
	virtual void Update();

private:

	const ComPtr<ID3D11DeviceContext> _context;
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11Buffer> _indexBuffer;
	unsigned int _vertexStride;
	unsigned int _vertexOffset{0};
	unsigned int _vertexCount{4};
};

