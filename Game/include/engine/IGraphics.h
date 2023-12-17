
#pragma once

#include <map>
#include <list>
#include <memory>

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

	virtual std::shared_ptr<ITexture> CreateTexture(const wchar_t* filepath) = 0;
	virtual std::shared_ptr<IShader> CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader, std::shared_ptr<ITexture> TextureIn) = 0;
	virtual std::shared_ptr<IRenderable> CreateBillboard(std::shared_ptr<IShader> ShaderIn) = 0;

	virtual void SetScreenSize(uint32_t width, uint32_t height) = 0;
	virtual void GetScreenSize(uint32_t& width, uint32_t& height) = 0;
	virtual void* GetRenderTextureSRV() const = 0;

protected:

	std::map<std::shared_ptr<IShader>, std::list<std::shared_ptr<IRenderable>>> _renderables;
	std::list<std::shared_ptr<ITexture>> _textures;

};

