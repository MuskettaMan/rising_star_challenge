
#pragma once

#include "Engine/IRenderable.h"
#include <d3d11.h>
#include <DirectXMath.h>

class DirectX11Graphics;

class DirectX11Billboard : public IRenderable
{
public:

	DirectX11Billboard(ID3D11DeviceContext* context, ID3D11Buffer* vertexBuffer, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCount);
	virtual ~DirectX11Billboard();
	virtual void Update();

private:

	ID3D11DeviceContext* _context;
	ID3D11Buffer* _vertexBuffer;
	unsigned int _vertexStride;
	unsigned int _vertexOffset;
	unsigned int _vertexCount;
};

