#include "pch.h"
#include "engine/implementation/directx11/DirectX11Billboard.h"
#include "engine/implementation/directx11/DirectX11Graphics.h"

DirectX11Billboard::DirectX11Billboard(const ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> vertexBuffer, ComPtr<ID3D11Buffer> indexBuffer, unsigned int vertexStride) : 
	_context(context), 
	_vertexBuffer(vertexBuffer), 
	_indexBuffer(indexBuffer), 
	_vertexStride(vertexStride)
{
}
	
void DirectX11Billboard::Update()
{
	if (_context)
	{
		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_context->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		_context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &_vertexStride, &_vertexOffset);
		_context->DrawIndexed(_vertexCount, 0, 0);
	}
}
