#include "pch.h"
#include "engine/implementation/directx11/DirectX11Billboard.h"
#include "engine/implementation/directx11/DirectX11Graphics.h"

DirectX11Billboard::DirectX11Billboard(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> vertexBuffer, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCount) : _context(context), _vertexBuffer(vertexBuffer), _vertexStride(vertexStride), _vertexOffset(vertexOffset), _vertexCount(vertexCount)
{
}
	
void DirectX11Billboard::Update()
{
	if (_context)
	{
		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &_vertexStride, &_vertexOffset);
		_context->Draw(_vertexCount, 0);
	}
}
