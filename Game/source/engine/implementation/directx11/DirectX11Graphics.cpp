#include "pch.h"
#include "engine/implementation/directx11/DirectX11Graphics.h"

#include "engine/implementation/directx11/DirectX11Billboard.h"
#include "engine/implementation/directx11/DirectX11Shader.h"
#include "engine/implementation/directx11/DirectX11Texture.h"

DirectX11Graphics::DirectX11Graphics(HWND hwndIn) : _device(nullptr), _context(nullptr), _swapChain(nullptr), _backbufferRTV(nullptr), _backbufferTexture(nullptr), _mvp(nullptr), _vpMatrix(), _featureLevel(D3D_FEATURE_LEVEL_11_0), _hwnd(hwndIn), _windowWidth(0), _windowHeight(0)
{
    RECT dimensions;
    GetClientRect(_hwnd, &dimensions);
    _windowWidth = dimensions.right - dimensions.left;
    _windowHeight = dimensions.bottom - dimensions.top;
    
    _renderTextureWidth = _windowWidth;
    _renderTextureHeight = _windowHeight;

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
        hr = SetupBackBuffer();
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

    SetupRenderTexture();

    ImGui_ImplDX11_Init(_device.Get(), _context.Get());
}

DirectX11Graphics::~DirectX11Graphics()
{
    ImGui_ImplDX11_Shutdown();
}

void DirectX11Graphics::BeginUpdate()
{
    ImGui_ImplDX11_NewFrame();
}

void DirectX11Graphics::Update()
{
    if (_context && _swapChain)
    {

        float clearColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        _context->ClearRenderTargetView(_backbufferRTV.Get(), clearColour);
        
        if(ENABLE_EDITOR)
            _context->ClearRenderTargetView(_renderTextureRTV.Get(), clearColour);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(_windowWidth);
        viewport.Height = static_cast<float>(_windowHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        _context->RSSetViewports(1, &viewport);


        _context->OMSetRenderTargets(1, ENABLE_EDITOR ? _renderTextureRTV.GetAddressOf() : _backbufferRTV.GetAddressOf(), nullptr);

        for (auto bucket = _renderables.begin(); bucket != _renderables.end(); ++bucket)
        {
            bucket->first->Update();
 
            for (auto renderable = bucket->second.begin(); renderable != bucket->second.end(); ++renderable)
            {
                SetWorldMatrix((*renderable)->GetTransform());
                _context->OMSetBlendState(_blendState.Get(), nullptr, ~0U);
                (*renderable)->Update();
            }
        }

        _context->OMSetRenderTargets(1, _backbufferRTV.GetAddressOf(), nullptr);
    }
}

void DirectX11Graphics::EndUpdate()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (_context && _swapChain)
    {
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
    ComPtr<ID3D11ShaderResourceView> srv = nullptr;
    ComPtr<ID3D11SamplerState> sampler = nullptr;
    ComPtr<ID3D11Texture2D> texture = nullptr;
    D3D11_TEXTURE2D_DESC description;

    if (IsValid())
    {
        HRESULT hr = DirectX::CreateDDSTextureFromFile(_device.Get(), filepath, NULL, srv.GetAddressOf());

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

            hr = _device->CreateSamplerState(&colorMapDesc, sampler.GetAddressOf());
        }

        if (SUCCEEDED(hr))
        {
            ComPtr<ID3D11Resource> textureResource;
            srv->GetResource(textureResource.GetAddressOf());

            textureResource.As(&texture);

            texture->GetDesc(&description);
        }

        if (SUCCEEDED(hr))
        {
            result = std::make_shared<DirectX11Texture>(_context, srv, sampler, texture, description);
            _textures.push_back(result);
        }
    }

    return result;
}

std::shared_ptr<IShader> DirectX11Graphics::CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader, std::shared_ptr<ITexture> textureIn)
{
    std::shared_ptr<IShader> result = nullptr;
    ComPtr<ID3D11VertexShader> vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> inputLayout = nullptr;
    HRESULT hr = S_FALSE;

    if (IsValid())
    {
        ComPtr<ID3DBlob> vsBuffer = nullptr;
        if (CompileShader(filepath, vsentry, vsshader, vsBuffer.GetAddressOf()))
        {
            hr = _device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, vertexShader.GetAddressOf());

            if(SUCCEEDED(hr))
            {
                D3D11_INPUT_ELEMENT_DESC layout[] =
                {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
                };

                unsigned int totalLayoutElements = ARRAYSIZE(layout);

                hr = _device->CreateInputLayout(layout, totalLayoutElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), inputLayout.GetAddressOf());
            }
        }

        if (SUCCEEDED(hr))
        {
            ComPtr<ID3DBlob> psBuffer = nullptr;
            hr = S_FALSE;
            if (CompileShader(filepath, psentry, psshader, psBuffer.GetAddressOf()))
            {
                hr = _device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, pixelShader.GetAddressOf());
            }
        }

        if (SUCCEEDED(hr))
        {
            result = std::make_shared<DirectX11Shader>(_context, vertexShader, pixelShader, inputLayout, textureIn);
            _renderables.insert(std::pair<std::shared_ptr<IShader>, std::list<std::shared_ptr<IRenderable>>>(result, std::list<std::shared_ptr<IRenderable>>()));
        }
    }

    return result;
}

std::shared_ptr<IRenderable> DirectX11Graphics::CreateBillboard(std::shared_ptr<IShader> shaderIn)
{
    std::shared_ptr<IRenderable> result = nullptr;

    if (IsValid())
    {
        const std::shared_ptr<ITexture> texture = shaderIn->GetTexture();
        const float halfWidth = texture ? texture->GetWidth() / 2.0f : 0.5f;
        const float halfHeight = texture ? texture->GetHeight() / 2.0f : 0.5f;

        BillboardVertex vertex_data_array[] =
        {
            XMFLOAT3{ -halfWidth,  -halfHeight, 0.0f }, XMFLOAT2{ 0.0f, 0.0f },
            XMFLOAT3{ halfWidth, -halfHeight, 0.0f }, XMFLOAT2{ 1.0f, 0.0f },
            XMFLOAT3{ halfWidth, halfHeight, 0.0f }, XMFLOAT2{ 1.0f, 1.0f },
            XMFLOAT3{ -halfWidth,  halfHeight, 0.0f }, XMFLOAT2{ 0.0f, 1.0f },
        };

        uint32_t index_data_array[] = {
            2, 1, 0,
            0, 3, 2
        };

        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;

        unsigned int vertexStride = sizeof(BillboardVertex);

        D3D11_BUFFER_DESC vertexDescription;
        ZeroMemory(&vertexDescription, sizeof(vertexDescription));
        vertexDescription.Usage = D3D11_USAGE_DEFAULT;
        vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexDescription.ByteWidth = sizeof(vertex_data_array);

        D3D11_BUFFER_DESC indexDescription;
        ZeroMemory(&indexDescription, sizeof(indexDescription));
        indexDescription.Usage = D3D11_USAGE_DEFAULT;
        indexDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexDescription.ByteWidth = sizeof(index_data_array);

        D3D11_SUBRESOURCE_DATA vertexResourceData;
        ZeroMemory(&vertexResourceData, sizeof(vertexResourceData));
        vertexResourceData.pSysMem = vertex_data_array;

        D3D11_SUBRESOURCE_DATA indexResourceData;
        ZeroMemory(&indexResourceData, sizeof(indexResourceData));
        indexResourceData.pSysMem = index_data_array;

        bool successVertexBuffer = SUCCEEDED(_device->CreateBuffer(&vertexDescription, &vertexResourceData, vertexBuffer.GetAddressOf()));
        bool successIndexBuffer = SUCCEEDED(_device->CreateBuffer(&indexDescription, &indexResourceData, indexBuffer.GetAddressOf()));

        if (successVertexBuffer && successIndexBuffer)
        {
            result = std::make_shared<DirectX11Billboard>(_context, vertexBuffer, indexBuffer, vertexStride);
            _renderables[shaderIn].push_back(result);
        }
    }

    return result;
}

void DirectX11Graphics::SetScreenSize(uint32_t width, uint32_t height)
{
    _windowWidth = width; _windowHeight = height;
    
    _context->OMSetRenderTargets(0, nullptr, nullptr);

    _backbufferRTV.Reset();
    _backbufferTexture.Reset();

    //_renderTexture.Reset();
    //_renderTextureRTV.Reset();
    //_renderTextureSRV.Reset();

    _context->Flush();

    _swapChain->ResizeBuffers(1, _windowWidth, _windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

    SetupBackBuffer();
    //SetupRenderTexture();

    float halfWidth = static_cast<float>(width / 2);
    float halfHeight = static_cast<float>(height / 2);
    DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 10.1f);
    _vpMatrix = DirectX::XMMatrixMultiply(view, projection);
}

void DirectX11Graphics::SetWorldMatrix(const Transform2D& transform)
{
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, 10.0f);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(transform.rotation);
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f);
    DirectX::XMMATRIX world = scale * rotation * translation;
    DirectX::XMMATRIX mvp = DirectX::XMMatrixMultiply(world, _vpMatrix);
    mvp = DirectX::XMMatrixTranspose(mvp);
    _context->UpdateSubresource(_mvp.Get(), 0, 0, &mvp, 0, 0);
    _context->VSSetConstantBuffers(0, 1, _mvp.GetAddressOf());
}

bool DirectX11Graphics::CompileShader(LPCWSTR filepath, LPCSTR entry, LPCSTR shader, ID3DBlob** buffer)
{
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> errorBuffer = 0;
    HRESULT hr = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shader, shaderFlags, 0, buffer, errorBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBuffer)
        {
            MessageBox(NULL, (char*)errorBuffer->GetBufferPointer(), "Error!", MB_ICONEXCLAMATION | MB_OK);
        }
    }

    return hr == S_OK;
}

HRESULT DirectX11Graphics::SetupBackBuffer()
{
    HRESULT hr;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(_backbufferTexture.GetAddressOf()));
    if (FAILED(hr))
        return hr;

    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    ZeroMemory(&desc, sizeof(desc));

    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = _device->CreateRenderTargetView(_backbufferTexture.Get(), &desc, _backbufferRTV.GetAddressOf());

    return hr;
}

void DirectX11Graphics::SetupRenderTexture()
{
    DXGI_SAMPLE_DESC sampleDesc;
    ZeroMemory(&sampleDesc, sizeof(sampleDesc));
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.Width = _renderTextureWidth;
    texDesc.Height = _renderTextureHeight;
    texDesc.CPUAccessFlags = 0;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc = sampleDesc;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = 0;
    texDesc.MipLevels = 1;
    _device->CreateTexture2D(&texDesc, nullptr, _renderTexture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    _device->CreateShaderResourceView(_renderTexture.Get(), &srvDesc, _renderTextureSRV.GetAddressOf());

    D3D11_RENDER_TARGET_VIEW_DESC rtvDescription;
    ZeroMemory(&rtvDescription, sizeof(rtvDescription));
    rtvDescription.Format = texDesc.Format;
    rtvDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDescription.Texture2D.MipSlice = 0;

    _device->CreateRenderTargetView(_renderTexture.Get(), &rtvDescription, _renderTextureRTV.GetAddressOf());
}
