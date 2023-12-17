#pragma once

#include "Engine/IGraphics.h"
#include "Engine/Transform2D.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11Buffer;
struct ID3D11BlendState;
enum D3D_FEATURE_LEVEL;

struct BillboardVertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;
};

class DirectX11Graphics : public IGraphics
{
public:

	DirectX11Graphics(HWND hwndIn);
	virtual ~DirectX11Graphics();

	virtual void BeginUpdate();
	virtual void Update();
	virtual void EndUpdate();
	virtual bool IsValid();

	virtual std::shared_ptr<ITexture> CreateTexture(const wchar_t* filepath);
	virtual std::shared_ptr<IShader> CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader, std::shared_ptr<ITexture> TextureIn);
	virtual std::shared_ptr<IRenderable> CreateBillboard(std::shared_ptr<IShader> ShaderIn);

	virtual void* GetRenderTextureSRV() const { return _renderTextureSRV.Get(); };
	virtual void SetScreenSize(uint32_t width, uint32_t height);
	virtual void GetScreenSize(uint32_t& width, uint32_t& height) { width = _windowWidth; height = _windowHeight; }

	ComPtr<ID3D11Device> GetDevice() const { return _device; }
	ComPtr<ID3D11DeviceContext> GetContext() const { return _context; }


protected:

	virtual void SetWorldMatrix(const Transform2D& transform);
	virtual bool CompileShader(LPCWSTR filepath, LPCSTR entry, LPCSTR shader, ID3DBlob** buffer);

private:

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11Texture2D> _backbufferTexture;
	ComPtr<ID3D11RenderTargetView> _backbufferRTV;
	ComPtr<ID3D11BlendState> _blendState;

	ComPtr<ID3D11Texture2D> _renderTexture;
	ComPtr<ID3D11ShaderResourceView> _renderTextureSRV;
	ComPtr<ID3D11RenderTargetView> _renderTextureRTV;

	ComPtr<ID3D11Buffer> _mvp;
	DirectX::XMMATRIX _vpMatrix;
	D3D_FEATURE_LEVEL _featureLevel;
	HWND _hwnd;
	uint32_t _windowWidth;
	uint32_t _windowHeight;
	uint32_t _renderTextureWidth;
	uint32_t _renderTextureHeight;
	
	HRESULT SetupBackBuffer();
	void SetupRenderTexture();
};

