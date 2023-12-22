#include "pch.h"
#include "engine/implementation/directx11/dx11_graphics.hpp"

#include <engine/camera.hpp>
#include <engine/ecs.hpp>
#include <engine/transform.hpp>
#include <engine/sprite_renderer.hpp>

DX11Graphics::DX11Graphics(HWND hwndIn) : _device(nullptr), _context(nullptr), _swapChain(nullptr), _backbufferRTV(nullptr), _backbufferTexture(nullptr), _mvp(nullptr), _featureLevel(D3D_FEATURE_LEVEL_11_0), _hwnd(hwndIn), _windowWidth(0), _windowHeight(0)
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

DX11Graphics::~DX11Graphics()
{
    ImGui_ImplDX11_Shutdown();
}

void DX11Graphics::BeginUpdate()
{
    ImGui_ImplDX11_NewFrame();

    auto view = ECS::Instance().Registry().view<Camera, CameraMatrix, TransformMatrix>();
    entt::entity entity = *view.begin();
    auto [camera, cameraMatrix, transformMatrix] = view.get(entity);

    float halfWidth = static_cast<float>(_windowWidth / 2) * camera.size;
    float halfHeight = static_cast<float>(_windowHeight / 2) * camera.size;

    transformMatrix.worldMatrix *= XMMatrixTranslationFromVector(XMVECTOR{ 0, 0, -10.0f });
    XMVECTOR determinant = XMMatrixDeterminant(transformMatrix.worldMatrix);
    cameraMatrix.view = DirectX::XMMatrixInverse(&determinant, transformMatrix.worldMatrix);
    cameraMatrix.projection = DirectX::XMMatrixOrthographicOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
}

void DX11Graphics::Update()
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


        auto spriteRendererView = ECS::Instance().Registry().view<const SpriteRenderer, const TransformMatrix>();
        for (const entt::entity spriteRendererEntity : spriteRendererView)
        {
            auto [spriteRenderer, transformMatrix]{spriteRendererView.get(spriteRendererEntity)};
            const DX11Mesh& mesh = _meshes[spriteRenderer.mesh.Id()];
            const DX11Shader& shader = _shaders[spriteRenderer.shader.Id()];
            const DX11Texture& texture = _textures[spriteRenderer.texture.Id()];

            _context->IASetInputLayout(shader._inputLayout.Get());
            _context->VSSetShader(shader._vertexShader.Get(), 0, 0);
            _context->PSSetShader(shader._pixelShader.Get(), 0, 0);

            _context->PSSetShaderResources(0, 1, texture._srv.GetAddressOf());
            _context->PSSetSamplers(0, 1, texture._sampler.GetAddressOf());

            _context->OMSetBlendState(_blendState.Get(), nullptr, ~0U);
            SetWorldMatrix(transformMatrix);

            _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _context->IASetIndexBuffer(mesh._indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _context->IASetVertexBuffers(0, 1, mesh._vertexBuffer.GetAddressOf(), &mesh._vertexStride, &mesh._vertexOffset);
            _context->DrawIndexed(mesh._vertexCount, 0, 0);
        }

        _context->OMSetRenderTargets(1, _backbufferRTV.GetAddressOf(), nullptr);
    }
}

void DX11Graphics::EndUpdate()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (_context && _swapChain)
    {
        _swapChain->Present(0, 0);
    }
}

bool DX11Graphics::IsValid()
{
    return _device != nullptr;
}

ResourceHandle<Texture> DX11Graphics::CreateTexture(const wchar_t* filepath)
{
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

            _textures.emplace_back(DX11Texture{ srv, sampler, texture, description });
            return ResourceHandle<Texture>(static_cast<uint32_t>(_textures.size() - 1));
        }
    }

    return ResourceHandle<Texture>();
}

ResourceHandle<Shader> DX11Graphics::CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader)
{
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

            if (SUCCEEDED(hr))
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

                _shaders.emplace_back(vertexShader, pixelShader, inputLayout);
                return ResourceHandle<Shader>(static_cast<uint32_t>(_shaders.size() - 1));
            }
        }
    }


    return ResourceHandle<Shader>();
}

ResourceHandle<Mesh> DX11Graphics::CreateBillboard(float width, float height)
{
    std::shared_ptr<IRenderable> result = nullptr;

    const float halfWidth = width / 2.0f;
    const float halfHeight = height / 2.0f;

    BillboardVertex vertex_data_array[] =
    {
        XMFLOAT3{ -halfWidth,  -halfHeight, 0.0f }, XMFLOAT2{ 0.0f, 1.0f },
        XMFLOAT3{ halfWidth, -halfHeight, 0.0f }, XMFLOAT2{ 1.0f, 1.0f },
        XMFLOAT3{ -halfWidth,  halfHeight, 0.0f }, XMFLOAT2{ 0.0f, 0.0f },
        XMFLOAT3{ halfWidth, halfHeight, 0.0f }, XMFLOAT2{ 1.0f, 0.0f },
    };

    uint32_t index_data_array[] = {
        0, 3, 1,
        0, 2, 3
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
        _meshes.emplace_back(vertexBuffer, indexBuffer, vertexStride);
        return ResourceHandle<Mesh>(static_cast<uint32_t>(_meshes.size() - 1));
    }

    return ResourceHandle<Mesh>();
}

void DX11Graphics::SetScreenSize(uint32_t width, uint32_t height)
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
}

void DX11Graphics::SetWorldMatrix(const TransformMatrix& transform)
{
    Camera& camera = ECS::Instance().GetCamera();
    CameraMatrix& cameraMatrix = ECS::Instance().GetCameraMatrix();

    XMMATRIX mvp = XMMatrixMultiply(transform.worldMatrix, DirectX::XMMatrixMultiply(cameraMatrix.view, cameraMatrix.projection));
    mvp = XMMatrixTranspose(mvp);
    _context->UpdateSubresource(_mvp.Get(), 0, 0, &mvp, 0, 0);
    _context->VSSetConstantBuffers(0, 1, _mvp.GetAddressOf());
}

bool DX11Graphics::CompileShader(LPCWSTR filepath, LPCSTR entry, LPCSTR shader, ID3DBlob** buffer)
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

HRESULT DX11Graphics::SetupBackBuffer()
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

void DX11Graphics::SetupRenderTexture()
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
