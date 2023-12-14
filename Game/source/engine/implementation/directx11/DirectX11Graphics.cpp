#include "pch.h"
#include "engine/implementation/directx11/DirectX11Graphics.h"

#include "engine/implementation/directx11/DirectX11Billboard.h"
#include "engine/implementation/directx11/DirectX11Shader.h"
#include "engine/implementation/directx11/DirectX11Texture.h"

DirectX11Graphics::DirectX11Graphics(HWND hwndIn) : _device(nullptr), _context(nullptr), _swapChain(nullptr), _backbufferView(nullptr), _backbufferTexture(nullptr), _mvp(nullptr), _vpMatrix(), _featureLevel(D3D_FEATURE_LEVEL_11_0), _hwnd(hwndIn), _windowWidth(0), _windowHeight(0)
{
    RECT dimensions;
    GetClientRect(_hwnd, &dimensions);
    _windowWidth = dimensions.right - dimensions.left;
    _windowHeight = dimensions.bottom - dimensions.top;

    D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE };
    unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _windowWidth;
    sd.BufferDesc.Height = _windowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    HRESULT hr = S_OK;
    unsigned int creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    for (unsigned int count = 0; count < totalDriverTypes; ++count)
    {
        hr = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[count], NULL, creationFlags, NULL, 0, D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &_featureLevel, &_context);

        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_backbufferTexture);
    }

    if (SUCCEEDED(hr))
    {
        hr = _device->CreateRenderTargetView(_backbufferTexture, 0, &_backbufferView);
    }

    if (_backbufferTexture)
    {
        _backbufferTexture->Release();
    }

    if (FAILED(hr))
    {
        MessageBox(NULL, "Graphics Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    else
    {
        D3D11_BUFFER_DESC constDesc;
        ZeroMemory(&constDesc, sizeof(constDesc));
        constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
        constDesc.Usage = D3D11_USAGE_DEFAULT;
        hr = _device->CreateBuffer(&constDesc, 0, &_mvp);

        if (FAILED(hr))
        {
            MessageBox(NULL, "Graphics Failed to create MVP Buffer", "Error!", MB_ICONEXCLAMATION | MB_OK);
        }

        float halfWidth = static_cast<float>(_windowWidth / 2);
        float halfHeight = static_cast<float>(_windowHeight / 2);
        DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 10.1f);
        _vpMatrix = DirectX::XMMatrixMultiply(view, projection);

        D3D11_BLEND_DESC Desc;
        ZeroMemory(&Desc, sizeof(D3D11_BLEND_DESC));
        Desc.RenderTarget[0].BlendEnable = TRUE;
        Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _device->CreateBlendState(&Desc, &_blendState);
    }
}

DirectX11Graphics::~DirectX11Graphics()
{
    if (_blendState)
    {
        _blendState->Release();
    }

    if (_backbufferView)
    {
        _backbufferView->Release();
    }

    if (_swapChain)
    {
        _swapChain->Release();
    }

    if (_context)
    {
        _context->Release();
    }

    if (_device)
    {
        _device->Release();
    }
}

void DirectX11Graphics::Update()
{
    if (_context && _swapChain)
    {
        float clearColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        _context->ClearRenderTargetView(_backbufferView, clearColour);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(_windowWidth);
        viewport.Height = static_cast<float>(_windowHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        _context->RSSetViewports(1, &viewport);

        _context->OMSetRenderTargets(1, &_backbufferView, NULL);

        for (auto bucket = _renderables.begin(); bucket != _renderables.end(); ++bucket)
        {
            bucket->first->Update();
 
            for (auto renderable = bucket->second.begin(); renderable != bucket->second.end(); ++renderable)
            {
                SetWorldMatrix((*renderable)->GetTransform());
                _context->OMSetBlendState(_blendState, NULL, ~0U);
                (*renderable)->Update();
            }
        }

        _swapChain->Present(0, 0);
    }
}

bool DirectX11Graphics::IsValid()
{
    return _device != nullptr;
}

std::shared_ptr<ITexture> DirectX11Graphics::CreateTexture(const wchar_t* filepath)
{
    std::shared_ptr<ITexture> result = nullptr;
    ID3D11ShaderResourceView* texture = nullptr;
    ID3D11SamplerState* sampler = nullptr;
    D3D11_TEXTURE2D_DESC description;

    if (IsValid())
    {
        HRESULT hr = DirectX::CreateDDSTextureFromFile(_device, filepath, NULL, &texture);

        if (SUCCEEDED(hr))
        {
            D3D11_SAMPLER_DESC colorMapDesc;
            ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
            colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

            hr = _device->CreateSamplerState(&colorMapDesc, &sampler);
        }

        if (SUCCEEDED(hr))
        {
            ID3D11Resource* textureResource;
            texture->GetResource(&textureResource);

            ((ID3D11Texture2D*)textureResource)->GetDesc(&description);
            textureResource->Release();
        }

        if (SUCCEEDED(hr))
        {
            result = std::make_shared<DirectX11Texture>(_context, texture, sampler, description);
            _textures.push_back(result);
        }
    }

    return result;
}

std::shared_ptr<IShader> DirectX11Graphics::CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader, std::shared_ptr<ITexture> TextureIn)
{
    std::shared_ptr<IShader> Result = nullptr;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    HRESULT hr = S_FALSE;
    ID3DBlob* vsBuffer = 0;

    if (IsValid())
    {
        if (CompileShader(filepath, vsentry, vsshader, &vsBuffer))
        {
            hr = _device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &VertexShader);

            if (FAILED(hr) && VertexShader)
            {
                VertexShader->Release();
            }
            else
            {
                D3D11_INPUT_ELEMENT_DESC layout[] =
                {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
                };

                unsigned int totalLayoutElements = ARRAYSIZE(layout);

                hr = _device->CreateInputLayout(layout, totalLayoutElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &InputLayout);
                vsBuffer->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            ID3DBlob* psBuffer = 0;
            hr = S_FALSE;
            if (CompileShader(filepath, psentry, psshader, &psBuffer))
            {
                hr = _device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PixelShader);
                psBuffer->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            Result = std::make_shared<DirectX11Shader>(_context, VertexShader, PixelShader, InputLayout, TextureIn);
            _renderables.insert(std::pair<std::shared_ptr<IShader>, std::list<std::shared_ptr<IRenderable>>>(Result, std::list<std::shared_ptr<IRenderable>>()));
        }
    }

    return Result;
}

std::shared_ptr<IRenderable> DirectX11Graphics::CreateBillboard(std::shared_ptr<IShader> ShaderIn)
{
    std::shared_ptr<IRenderable> Result = nullptr;

    if (IsValid())
    {
        const std::shared_ptr<ITexture> texture = ShaderIn->GetTexture();
        const float halfWidth = texture ? texture->GetWidth() / 2.0f : 0.5f;
        const float halfHeight = texture ? texture->GetHeight() / 2.0f : 0.5f;

        float vertex_data_array[] =
        {
            halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f,
            halfWidth, -halfHeight, 0.0f,  1.0f, 0.0f,
           -halfWidth, -halfHeight, 0.0f,  0.0f, 0.0f,

           -halfWidth, -halfHeight, 0.0f,  0.0f, 0.0f,
           -halfWidth,  halfHeight, 0.0f,  0.0f, 1.0f,
            halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f,
        };

        ID3D11Buffer* VertexBuffer;
        unsigned int vertexStride = 5 * sizeof(float);
        unsigned int vertexOffset = 0;
        unsigned int vertexCount = 6;

        D3D11_BUFFER_DESC vertexDescription;
        ZeroMemory(&vertexDescription, sizeof(vertexDescription));
        vertexDescription.Usage = D3D11_USAGE_DEFAULT;
        vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexDescription.ByteWidth = sizeof(vertex_data_array);

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(resourceData));
        resourceData.pSysMem = vertex_data_array;

        if (SUCCEEDED(_device->CreateBuffer(&vertexDescription, &resourceData, &VertexBuffer)))
        {
            Result = std::make_shared<DirectX11Billboard>(_context, VertexBuffer, vertexStride, vertexOffset, vertexCount);
            _renderables[ShaderIn].push_back(Result);
        }
    }

    return Result;
}

void DirectX11Graphics::SetWorldMatrix(const Transform2D& transform)
{
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(transform.positionX, transform.positionY, 10.0f);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(transform.rotation);
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(transform.scaleX, transform.scaleY, 1.0f);
    DirectX::XMMATRIX world = scale * rotation * translation;
    DirectX::XMMATRIX mvp = DirectX::XMMatrixMultiply(world, _vpMatrix);
    mvp = DirectX::XMMatrixTranspose(mvp);
    _context->UpdateSubresource(_mvp, 0, 0, &mvp, 0, 0);
    _context->VSSetConstantBuffers(0, 1, &_mvp);
}

bool DirectX11Graphics::CompileShader(LPCWSTR filepath, LPCSTR entry, LPCSTR shader, ID3DBlob** buffer)
{
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBuffer = 0;
    HRESULT hr = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shader, shaderFlags, 0, buffer, &errorBuffer);

    if (FAILED(hr))
    {
        if (errorBuffer)
        {
            MessageBox(NULL, (char*)errorBuffer->GetBufferPointer(), "Error!", MB_ICONEXCLAMATION | MB_OK);
        }
    }

    if (errorBuffer)
    {
        errorBuffer->Release();
    }

    return hr == S_OK;
}
