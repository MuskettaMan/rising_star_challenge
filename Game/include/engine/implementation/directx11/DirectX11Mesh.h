#pragma once

#include "engine/mesh.hpp"

struct DirectX11Mesh : public Mesh
{
	DirectX11Mesh(const ComPtr<ID3D11Buffer>& _vertexBuffer, const ComPtr<ID3D11Buffer>& _indexBuffer, unsigned int _vertexStride)
		: _vertexBuffer(_vertexBuffer), _indexBuffer(_indexBuffer), _vertexStride(_vertexStride)
	{
	}

	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11Buffer> _indexBuffer;
	unsigned int _vertexStride;
	unsigned int _vertexOffset{ 0 };
	unsigned int _vertexCount{ 6 };
};