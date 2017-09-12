#include <windows.h>

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// STL includes
#include <algorithm>
#include <iostream>
#include <string>

#include "utils.h"
#include "Resource.h"

using namespace DirectX;

const LONG gWindowWidth     = 1280;
const LONG gWindowHeight    = 720;

LPCWSTR gWindowClassName    = L"DirectXWindowClass";
LPCWSTR gWindowName         = L"DirectX Template";
HWND gWindowHandle          = 0;

const BOOL gEnableVSync    = FALSE;

// Direct3D device and swap chain.
ID3D11Device*               gD3DDevice             = nullptr;
ID3D11DeviceContext*        gD3DDeviceContext      = nullptr;
IDXGISwapChain*             gD3DSwapChain          = nullptr;

// Render target view for the back buffer of the swap chain.
ID3D11RenderTargetView*     gD3DRenderTargetView   = nullptr;

// Depth/stencil view for use as a depth buffer.
ID3D11DepthStencilView*     gD3DDepthStencilView   = nullptr;

// A texture to associate to the depth stencil view.
ID3D11Texture2D*            gD3DDepthStencilBuffer = nullptr;

// Define the functionality of the depth/stencil stages.
ID3D11DepthStencilState*    gD3DDepthStencilState  = nullptr;

// Define the functionality of the rasterizer stage.
ID3D11RasterizerState*      gD3DRasterizerState    = nullptr;

// Vertex buffer data
ID3D11InputLayout*          gD3DInputLayout        = nullptr;
ID3D11Buffer*               gD3DVertexBuffer       = nullptr;
ID3D11Buffer*               gD3DIndexBuffer        = nullptr;

// Shader data
ID3D11VertexShader*         gD3DVertexShader       = nullptr;
ID3D11PixelShader*          gD3DPixelShader        = nullptr;

D3D11_VIEWPORT              gViewport = { 0 };


// Shader resources
enum ConstanBuffer
{
    CB_Appliation,
    CB_Frame,
    CB_Object,
    NumConstantBuffers
};

ID3D11Buffer* gD3DConstantBuffers[NumConstantBuffers];

// Demo parameters
XMMATRIX gWorldMatrix;
XMMATRIX gViewMatrix;
XMMATRIX gProjectionMatrix;

// Vertex data for a colored cube.
struct VertexNormalUV
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};

VertexNormalUV gVertices[8] =
{
    { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, 0.0f ) }, // 0
    { XMFLOAT3( -1.0f,  1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) }, // 1
    { XMFLOAT3(  1.0f,  1.0f, -1.0f ), XMFLOAT3( 1.0f, 1.0f, 0.0f ) }, // 2
    { XMFLOAT3(  1.0f, -1.0f, -1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) }, // 3
    { XMFLOAT3( -1.0f, -1.0f,  1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) }, // 4
    { XMFLOAT3( -1.0f,  1.0f,  1.0f ), XMFLOAT3( 0.0f, 1.0f, 1.0f ) }, // 5
    { XMFLOAT3(  1.0f,  1.0f,  1.0f ), XMFLOAT3( 1.0f, 1.0f, 1.0f ) }, // 6
    { XMFLOAT3(  1.0f, -1.0f,  1.0f ), XMFLOAT3( 1.0f, 0.0f, 1.0f ) }  // 7
};

WORD gIndicies[36] =
{
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};


// Forward declarations.
int InitApplication(HINSTANCE hInstance, int cmdShow);
int InitDirectX(HINSTANCE hInstance, BOOL vSync);

std::string GetLatestVSProfile();
std::string GetLatestPSProfile();


ID3D11VertexShader* CreateVertexShaderFromFile(const std::wstring& filename, const std::string& entryPoint);
ID3D11PixelShader* CreatePixelShaderFromFile(const std::wstring& filename, const std::string& entryPoint);

ID3D11VertexShader* CreateVertexShader(ID3DBlob* shaderBlob);
ID3D11PixelShader* CreatePixelShader(ID3DBlob* shaderBlob);

void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
int Run();
bool GenerateContent();
void Update(float deltaTime);
void Render();
void Present(bool vSync);
void Cleanup();

DXGI_RATIONAL DetermineRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync);

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    if (!XMVerifyCPUSupport())
    {
        MessageBox(nullptr, TEXT("Failed to verify DirectX Math Library support."), TEXT("Error"), MB_OK);
        return -1;
    }

    if (InitApplication(hInstance, cmdShow) != 0)
    {
        MessageBox(nullptr, TEXT("Failed to create application window"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (InitDirectX(hInstance, gEnableVSync) != 0)
    {
        MessageBox(nullptr, TEXT("Failed to initialize DirectX 11"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!GenerateContent())
    {
        MessageBox(nullptr, TEXT("Failed to generate data for rendering"), TEXT("Error"), MB_OK);
        return -1;
    }

    int result = Run();

    Cleanup();
    return result;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paintStruct;
    HDC hDC;

    switch (message)
    {
    case WM_PAINT:
        {
            hDC = BeginPaint(hwnd, &paintStruct);
            EndPaint(hwnd, &paintStruct);
        }
        break;
    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

int InitApplication(HINSTANCE hInstance, int cmdShow)
{
    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTBED));
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = gWindowClassName;

    if (!RegisterClassEx(&wndClass))
    {
        return -1;
    }

    RECT windowRect = { 0, 0, gWindowWidth, gWindowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    gWindowHandle = CreateWindowW(gWindowClassName, gWindowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!gWindowHandle)
    {
        return -1;
    }

    ShowWindow(gWindowHandle, cmdShow);
    UpdateWindow(gWindowHandle);

    return 0;
}
int InitDirectX(HINSTANCE hInstance, BOOL vSync)
{
    UNREFERENCED_PARAMETER(hInstance);

    // A window handle must have been created already.
    assert(gWindowHandle != 0);

    RECT clientRect;
    GetClientRect(gWindowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate = DetermineRefreshRate(clientWidth, clientHeight, vSync);
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = gWindowHandle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
#if _DEBUG
    createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    // These are the feature levels that we will accept.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // This will be the feature level that
    // is used to create our device and swap chain.
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
        D3D11_SDK_VERSION, &swapChainDesc, &gD3DSwapChain, &gD3DDevice, &featureLevel,
        &gD3DDeviceContext);

    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
            D3D11_SDK_VERSION, &swapChainDesc, &gD3DSwapChain, &gD3DDevice, &featureLevel,
            &gD3DDeviceContext);
    }

    if (FAILED(hr))
    {
        return -1;
    }

    // The Direct3D device and swap chain were successfully created.
    // Now we need to initialize the buffers of the swap chain.
    // Next initialize the back buffer of the swap chain and associate it to a
    // render target view.
    ID3D11Texture2D* backBuffer;
    hr = gD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = gD3DDevice->CreateRenderTargetView(backBuffer, nullptr, &gD3DRenderTargetView);
    if (FAILED(hr))
    {
        return -1;
    }

    SafeRelease(backBuffer);

    // Create the depth buffer for use with the depth/stencil view.
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.Width = clientWidth;
    depthStencilBufferDesc.Height = clientHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = gD3DDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &gD3DDepthStencilBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = gD3DDevice->CreateDepthStencilView(gD3DDepthStencilBuffer, nullptr, &gD3DDepthStencilView);
    if (FAILED(hr))
    {
        return -1;
    }

    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = FALSE;

    hr = gD3DDevice->CreateDepthStencilState(&depthStencilStateDesc, &gD3DDepthStencilState);

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state object.
    hr = gD3DDevice->CreateRasterizerState(&rasterizerDesc, &gD3DRasterizerState);
    if (FAILED(hr))
    {
        return -1;
    }

    // Initialize the viewport to occupy the entire client area.
    gViewport.Width = static_cast<float>(clientWidth);
    gViewport.Height = static_cast<float>(clientHeight);
    gViewport.TopLeftX = 0.0f;
    gViewport.TopLeftY = 0.0f;
    gViewport.MinDepth = 0.0f;
    gViewport.MaxDepth = 1.0f;

    return 0;
}

std::string GetLatestVSProfile()
{
    assert(gD3DDevice != nullptr);

    D3D_FEATURE_LEVEL level = gD3DDevice->GetFeatureLevel();

    switch (level)
    {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            return "vs_5_0";
            break;
        }

        default:
        {
            return "";
            break;
        }
    }
}

std::string GetLatestPSProfile()
{
    assert(gD3DDevice != nullptr);

    D3D_FEATURE_LEVEL level = gD3DDevice->GetFeatureLevel();

    switch (level)
    {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            return "ps_5_0";
            break;
        }

        default:
        {
            return "";
            break;
        }
    }
}

DXGI_RATIONAL DetermineRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
    DXGI_RATIONAL refreshRate = { 0, 1 };
    if ( vsync )
    {
        IDXGIFactory* factory;
        IDXGIAdapter* adapter;
        IDXGIOutput* adapterOutput;
        DXGI_MODE_DESC* displayModeList;

        // Create a DirectX graphics interface factory.
        HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
        if ( FAILED(hr) )
        {
            MessageBox(0,
                TEXT("Could not create DXGIFactory instance."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to create DXGIFactory.");
        }

        hr = factory->EnumAdapters(0, &adapter);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to enumerate adapters."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to enumerate adapters.");
        }

        hr = adapter->EnumOutputs(0, &adapterOutput);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to enumerate adapter outputs."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to enumerate adapter outputs.");
        }

        UINT numDisplayModes;
        hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to query display mode list."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to query display mode list.");
        }

        displayModeList = new DXGI_MODE_DESC[numDisplayModes];
        assert(displayModeList);

        hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to query display mode list."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to query display mode list.");
        }

        // Now store the refresh rate of the monitor that matches the width and height of the requested screen.
        for (UINT i = 0; i < numDisplayModes; ++i)
        {
            if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
            {
                refreshRate = displayModeList[i].RefreshRate;
            }
        }

        delete [] displayModeList;
        SafeRelease(adapterOutput);
        SafeRelease(adapter);
        SafeRelease(factory);
    }

    return refreshRate;
}


bool GenerateContent()
{
    assert(gD3DDevice);

    // Create an initialize the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(VertexNormalUV) * _countof(gVertices);
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

    resourceData.pSysMem = gVertices;

    HRESULT hr = gD3DDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &gD3DVertexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    // Create and initialize the index buffer.
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(gIndicies);
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    resourceData.pSysMem = gIndicies;

    hr = gD3DDevice->CreateBuffer(&indexBufferDesc, &resourceData, &gD3DIndexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the constant buffers for the variables defined in the vertex shader.
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = gD3DDevice->CreateBuffer(&constantBufferDesc, nullptr, &gD3DConstantBuffers[CB_Appliation]);
    if (FAILED(hr))
    {
        return false;
    }
    hr = gD3DDevice->CreateBuffer(&constantBufferDesc, nullptr, &gD3DConstantBuffers[CB_Frame]);
    if (FAILED(hr))
    {
        return false;
    }
    hr = gD3DDevice->CreateBuffer(&constantBufferDesc, nullptr, &gD3DConstantBuffers[CB_Object]);
    if (FAILED(hr))
    {
        return false;
    }

    // Load the shaders
    gD3DVertexShader = CreateVertexShaderFromFile(L"./data/shaders/SimpleVertexShader.hlsl", "SimpleVertexShader");
    gD3DPixelShader = CreatePixelShaderFromFile(L"./data/shaders/SimplePixelShader.hlsl", "SimplePixelShader");

    // Setup the projection matrix.
    RECT clientRect;
    GetClientRect(gWindowHandle, &clientRect);

    // Compute the exact client dimensions.
    // This is required for a correct projection matrix.
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    gProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth/clientHeight, 0.1f, 100.0f);

    gD3DDeviceContext->UpdateSubresource(gD3DConstantBuffers[CB_Appliation], 0, nullptr, &gProjectionMatrix, 0, 0);

    return true;
}

// Clear the color and depth buffers.
void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    gD3DDeviceContext->ClearRenderTargetView(gD3DRenderTargetView, clearColor);
    gD3DDeviceContext->ClearDepthStencilView(gD3DDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Present(bool vSync)
{
    if ( vSync )
    {
        gD3DSwapChain->Present( 1, 0 );
    }
    else
    {
        gD3DSwapChain->Present( 0, 0 );
    }
}

void Update(float deltaTime)
{
    XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
    XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    gViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    gD3DDeviceContext->UpdateSubresource(gD3DConstantBuffers[CB_Frame], 0, nullptr, &gViewMatrix, 0, 0);


    static float angle = 0.0f;
    angle += 90.0f * deltaTime;
    XMVECTOR rotationAxis = XMVectorSet( 0, 1, 1, 0 );

    gWorldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle) );
    gD3DDeviceContext->UpdateSubresource(gD3DConstantBuffers[CB_Object], 0, nullptr, &gWorldMatrix, 0, 0);
}

void Render()
{
    assert(gD3DDevice);
    assert(gD3DDeviceContext);

    Clear(Colors::CornflowerBlue, 1.0f, 0);

    const UINT vertexStride = sizeof(VertexNormalUV);
    const UINT offset = 0;

    gD3DDeviceContext->VSSetShader(gD3DVertexShader, nullptr, 0);
    gD3DDeviceContext->VSSetConstantBuffers(0, 3, gD3DConstantBuffers);

    gD3DDeviceContext->RSSetState(gD3DRasterizerState);
    gD3DDeviceContext->RSSetViewports(1, &gViewport);

    gD3DDeviceContext->PSSetShader(gD3DPixelShader, nullptr, 0);

    gD3DDeviceContext->OMSetRenderTargets(1, &gD3DRenderTargetView, gD3DDepthStencilView);
    gD3DDeviceContext->OMSetDepthStencilState(gD3DDepthStencilState, 1);

    gD3DDeviceContext->IASetVertexBuffers(0, 1, &gD3DVertexBuffer, &vertexStride, &offset);
    gD3DDeviceContext->IASetInputLayout(gD3DInputLayout);
    gD3DDeviceContext->IASetIndexBuffer(gD3DIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    gD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    gD3DDeviceContext->DrawIndexed(_countof(gIndicies), 0, 0);

    Present(gEnableVSync);
}

void Cleanup()
{
    SafeRelease(gD3DConstantBuffers[CB_Appliation]);
    SafeRelease(gD3DConstantBuffers[CB_Frame]);
    SafeRelease(gD3DConstantBuffers[CB_Object]);
    SafeRelease(gD3DIndexBuffer);
    SafeRelease(gD3DVertexBuffer);
    SafeRelease(gD3DInputLayout);
    SafeRelease(gD3DVertexShader);
    SafeRelease(gD3DPixelShader);

    SafeRelease(gD3DDepthStencilView);
    SafeRelease(gD3DRenderTargetView);
    SafeRelease(gD3DDepthStencilBuffer);
    SafeRelease(gD3DDepthStencilState);
    SafeRelease(gD3DRasterizerState);
    SafeRelease(gD3DSwapChain);
    SafeRelease(gD3DDeviceContext);
    SafeRelease(gD3DDevice);
}

int Run()
{
    MSG msg = {0};

    static DWORD previousTime = timeGetTime();
    static const float targetFramerate = 30.0f;
    static const float maxTimeStep = 1.0f / targetFramerate;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = timeGetTime();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            // Cap the delta time to the max time step (useful if your
            // debugging and you don't want the deltaTime value to explode.
            deltaTime = std::min<float>(deltaTime, maxTimeStep);

            Update(deltaTime);
            Render();
        }
    }

    return static_cast<int>(msg.wParam);
}

ID3D11VertexShader* CreateVertexShaderFromFile(const std::wstring& filename, const std::string& entryPoint)
{
    ID3D11VertexShader* shader = nullptr;
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    std::string profile = GetLatestVSProfile();

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif
    HRESULT hr = D3DCompileFromFile(filename.c_str(),
                                    nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    entryPoint.c_str(),
                                    profile.c_str(),
                                    flags,
                                    0,
                                    &shaderBlob,
                                    &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob != nullptr)
        {
            std::string errorMessage = (char*)errorBlob->GetBufferPointer();
            OutputDebugStringA( errorMessage.c_str() );
        }
        else
        {
            OutputDebugStringA("Unknown Vertex Shader Compile error!");
        }
    }
    else
    {
        // Create the input layout for the vertex shader.
        D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexNormalUV,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexNormalUV,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        hr = gD3DDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &gD3DInputLayout);
        if (FAILED(hr))
        {
            OutputDebugStringA("Unable to generate Input layout for shader!");
            return false;
        }
        else
        {
            shader = CreateVertexShader(shaderBlob);
        }

    }


    SafeRelease(shaderBlob);
    SafeRelease(errorBlob);

    return shader;
}

ID3D11PixelShader* CreatePixelShaderFromFile(const std::wstring& filename, const std::string& entryPoint)
{
    ID3D11PixelShader* shader = nullptr;
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    std::string profile = GetLatestPSProfile();

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif
    HRESULT hr = D3DCompileFromFile(filename.c_str(),
                                    nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    entryPoint.c_str(),
                                    profile.c_str(),
                                    flags,
                                    0,
                                    &shaderBlob,
                                    &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob != nullptr)
        {
            std::string errorMessage = (char*)errorBlob->GetBufferPointer();
            OutputDebugStringA( errorMessage.c_str() );
        }
        else
        {
            OutputDebugStringA("Unknown Pixel shader compile error!");
        }
    }
    else
    {
        shader = CreatePixelShader(shaderBlob);
    }

    SafeRelease(shaderBlob);
    SafeRelease(errorBlob);
    return shader;
}

ID3D11VertexShader* CreateVertexShader(ID3DBlob* shaderBlob)
{
    assert(gD3DDevice);
    assert(shaderBlob);

    ID3D11VertexShader* result = nullptr;
    gD3DDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &result);
    return result;
}

ID3D11PixelShader* CreatePixelShader(ID3DBlob* shaderBlob)
{
    assert(gD3DDevice);
    assert(shaderBlob);

    ID3D11PixelShader* result = nullptr;
    gD3DDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &result);
    return result;
}
