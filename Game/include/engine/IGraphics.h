
#pragma once

#include <map>
#include <list>
#include <memory>
#include "engine/texture.hpp"
#include "engine/shader.hpp"
#include "engine/mesh.hpp"

class IRenderable;
class IShader;
class ITexture;

class IGraphics
{
public:

	IGraphics();
	virtual ~IGraphics();

	virtual void BeginUpdate() = 0;
	virtual void Update() = 0;
	virtual void EndUpdate() = 0;
	virtual bool IsValid() = 0;

	virtual ResourceHandle<Texture> CreateTexture(const wchar_t* filepath) = 0;
	virtual ResourceHandle<Shader> CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader) = 0;
	virtual ResourceHandle<Mesh> CreateBillboard(float width, float height) = 0;
	virtual void DrawLine(XMFLOAT2 from, XMFLOAT2 to, XMFLOAT3 color = {1.0f, 1.0f, 1.0f}) = 0;

	virtual void SetScreenSize(uint32_t width, uint32_t height) = 0;
	virtual void GetScreenSize(uint32_t& width, uint32_t& height) = 0;
	virtual void* GetRenderTextureSRV() const = 0;

};

