/// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

/// Header for the class
#include "RenderDevice.h"

/// And any utilities
#include "utils\utils.h"

RenderDevice::RenderDevice()
{

}

RenderDevice::~RenderDevice()
{
    ReleaseAll();
}

int RenderDevice::InitDevice(HWND hWnd, bool vSync)
{
    UNREFERENCED_PARAMETER(vSync);
    mBoundHWnd = hWnd;

    RECT clientRect;
    GetClientRect(mBoundHWnd, &clientRect);

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
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = mBoundHWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
// #if _DEBUG
    // This appears to no longer work in Windows 10
//     createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
// #endif

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

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               createDeviceFlags,
                                               featureLevels,
                                               _countof(featureLevels),
                                               D3D11_SDK_VERSION,
                                               &swapChainDesc,
                                               &mD3DSwapChain,
                                               &mD3DDevice,
                                               &featureLevel,
                                               &mD3DDeviceContext);

    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                           D3D_DRIVER_TYPE_HARDWARE,
                                           nullptr,
                                           createDeviceFlags,
                                           &featureLevels[1],
                                           _countof(featureLevels) - 1,
                                           D3D11_SDK_VERSION,
                                           &swapChainDesc,
                                           &mD3DSwapChain,
                                           &mD3DDevice,
                                           &featureLevel,
                                           &mD3DDeviceContext);
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
    hr = mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = mD3DDevice->CreateRenderTargetView(backBuffer, nullptr, &mD3DRenderTargetView);
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

    hr = mD3DDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &mD3DDepthStencilBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = mD3DDevice->CreateDepthStencilView(mD3DDepthStencilBuffer, nullptr, &mD3DDepthStencilView);
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

    hr = mD3DDevice->CreateDepthStencilState(&depthStencilStateDesc, &mD3DDepthStencilState);

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
    hr = mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mD3DRasterizerState);
    if (FAILED(hr))
    {
        return -1;
    }

    // Initialize the viewport to occupy the entire client area.
    mViewport = new D3D11_VIEWPORT();
    mViewport->Width = static_cast<float>(clientWidth);
    mViewport->Height = static_cast<float>(clientHeight);
    mViewport->TopLeftX = 0.0f;
    mViewport->TopLeftY = 0.0f;
    mViewport->MinDepth = 0.0f;
    mViewport->MaxDepth = 1.0f;

    return 0;
}

void RenderDevice::ReleaseAll()
{
    SafeRelease(mD3DRenderTargetView);
    SafeRelease(mD3DRasterizerState);
    SafeRelease(mD3DDepthStencilState);
    SafeRelease(mD3DDepthStencilView);
    SafeRelease(mD3DDepthStencilBuffer);
    SafeRelease(mD3DSwapChain);
    SafeRelease(mD3DDeviceContext);

    #ifdef _DEBUG
    ID3D11Debug* debugDevice = nullptr;
    HRESULT hr = mD3DDevice->QueryInterface(IID_PPV_ARGS(&debugDevice));

    if (SUCCEEDED(hr))
        debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    SafeRelease(debugDevice);
    #endif

    delete mViewport;
    mViewport = nullptr;

    SafeRelease(mD3DDevice);
}

void RenderDevice::Shutdown()
{
    ReleaseAll();
}

void RenderDevice::Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    mD3DDeviceContext->ClearRenderTargetView(mD3DRenderTargetView, clearColor);
    mD3DDeviceContext->ClearDepthStencilView(mD3DDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void RenderDevice::Present(bool vSync)
{
    if ( vSync )
    {
        mD3DSwapChain->Present( 1, 0 );
    }
    else
    {
        mD3DSwapChain->Present( 0, 0 );
    }
}