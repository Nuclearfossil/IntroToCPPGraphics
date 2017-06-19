#include "stdafx.h"

/// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "graphics\RenderDevice.h"
#include "graphics\ShaderManager.h"

#include "utils\utils.h"
#include "utils\assert.h"

ShaderManager::ShaderManager()
    : mRenderDevice(nullptr)
{
}

ShaderManager::~ShaderManager()
{
    mRenderDevice = nullptr; /// Don't delete as it's being managed elsewhere
}

bool ShaderManager::Init(RenderDevice* device)
{
    DetermineSupportedShaderLevel(device);
    mRenderDevice = device;

    ID3D11Device* d3dDevice = mRenderDevice->GetD3D11();

    // Create the constant buffers for the variables defined in the vertex shader.
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &mD3DConstantBuffers[CB_Appliation]);
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &mD3DConstantBuffers[CB_Frame]);
    if (FAILED(hr))
        return false;

    hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &mD3DConstantBuffers[CB_Object]);
    if (FAILED(hr))
        return false;

    return true;
}

void ShaderManager::Cleanup()
{
    SafeRelease(mD3DConstantBuffers[CB_Appliation]);
    SafeRelease(mD3DConstantBuffers[CB_Frame]);
    SafeRelease(mD3DConstantBuffers[CB_Object]);
    SafeRelease(mD3DInputLayout);
    SafeRelease(mD3DVertexShader);
    SafeRelease(mD3DPixelShader);
}

const char* ShaderManager::CurrentlySupportedShader(ShaderType shaderType)
{
    switch (shaderType)
    {
    case Vertex:
        switch (mSupportedVertexShaderLevel)
        {
        case PSM_5_0:
            return "vs_5_0";
        default:
            break;
        }
        break;

    case Pixel:
        switch (mSupportedPixelShaderLevel)
        {
        case VSM_5_0:
            return "ps_5_0";
        default:
            break;
        }
        break;

    case Compute:
    case Undefined:
    default:
        break;
    }

    return "undefined";
}

HRESULT ShaderManager::LoadVSFromFile(LPCWSTR filename, const char* mainfunction)
{
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexNormalUV,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexNormalUV,Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexNormalUV,TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    const char* vertexShaderProfile = CurrentlySupportedShader(ShaderType::Vertex);

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile(filename,
                                    nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE, 
                                    mainfunction,
                                    vertexShaderProfile,
                                    flags,
                                    0,
                                    &shaderBlob,
                                    &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob != nullptr)
        {
            const char* errorMessage = (char*)errorBlob->GetBufferPointer();
            OutputDebugStringA(errorMessage);
        }
        else
        {
            OutputDebugStringA("Unknown Vertex Shader Compile error!");
        }
    }
    else
    {
        hr = mRenderDevice->GetD3D11()->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &mD3DInputLayout);

        if (FAILED(hr))
        {
            OutputDebugStringA("Unable to generate Input layout for shader!");
            return false;
        }
        else
        {
            mD3DVertexShader = CreateVertexShader(shaderBlob);
        }
    }
    SafeRelease(shaderBlob);
    SafeRelease(errorBlob);

    return hr;
}

HRESULT ShaderManager::LoadPSFromFile(LPCWSTR filename, const char* mainfunction)
{
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    const char* profile = CurrentlySupportedShader(ShaderType::Pixel);

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif
    HRESULT hr = D3DCompileFromFile(filename, 
                                    nullptr, 
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE, 
                                    mainfunction,
                                    profile,
                                    flags,
                                    0,
                                    &shaderBlob,
                                    &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob != nullptr)
        {
            const char* errorMessage = (char*)errorBlob->GetBufferPointer();
            OutputDebugStringA(errorMessage);
        }
        else
        {
            OutputDebugStringA("Unknown Pixel shader compile error!");
        }
    }
    else
    {
        mD3DPixelShader = CreatePixelShader(shaderBlob);
    }

    SafeRelease(shaderBlob);
    SafeRelease(errorBlob);
    return hr;
}

ID3D11VertexShader* ShaderManager::CreateVertexShader(ID3DBlob* shaderBlob)
{
    assert(mRenderDevice);
    assert(shaderBlob);

    ID3D11VertexShader* result = nullptr;
    mRenderDevice->GetD3D11()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &result);
    return result;
}

ID3D11PixelShader* ShaderManager::CreatePixelShader(ID3DBlob* shaderBlob)
{
    assert(mRenderDevice);
    assert(shaderBlob);

    ID3D11PixelShader* result = nullptr;
    mRenderDevice->GetD3D11()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &result);
    return result;
}

void ShaderManager::DetermineSupportedShaderLevel(RenderDevice* device)
{
    assert(device != nullptr);
    D3D_FEATURE_LEVEL level = device->GetD3D11()->GetFeatureLevel();
    switch (level)
    {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            mSupportedPixelShaderLevel = PSM_5_0;
            mSupportedVertexShaderLevel = VSM_5_0;
            break;
        }

        default:
        {
            mSupportedPixelShaderLevel = PSM_UNDEFINED;
            mSupportedVertexShaderLevel = VSM_UNDEFINED;
            break;
        }
    }
}

bool ShaderManager::UpdateProjection(HWND hWnd)
{
    // Setup the projection matrix.
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Compute the exact client dimensions.
    // This is required for a correct projection matrix.
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    DirectX::XMMATRIX projectionMatrix;
    projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), clientWidth/clientHeight, 0.1f, 100.0f);

    ID3D11DeviceContext* context = nullptr;
    mRenderDevice->GetD3D11()->GetImmediateContext(&context);
    context->UpdateSubresource(mD3DConstantBuffers[CB_Appliation], 0, nullptr, &projectionMatrix, 0, 0);

    return true;
}