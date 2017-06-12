#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct D3D11_VIEWPORT;


class RenderDevice
{
private:
    void ReleaseAll();

public:
    RenderDevice();
    ~RenderDevice();

    int InitDevice(HWND hWnd, bool vSync);
    void Shutdown();
    void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);

    void Present(bool vSync);

    ID3D11Device* GetD3D11() { return mD3DDevice; }

private:
    bool mEnableVSync;
    HWND mBoundHWnd;

    // Direct3D device and swap chain.
    ID3D11Device*               mD3DDevice             = nullptr;
    ID3D11DeviceContext*        mD3DDeviceContext      = nullptr;
    IDXGISwapChain*             mD3DSwapChain          = nullptr;

    // Render target view for the back buffer of the swap chain.
    ID3D11RenderTargetView*     mD3DRenderTargetView   = nullptr;

    // Depth/stencil view for use as a depth buffer.
    ID3D11DepthStencilView*     mD3DDepthStencilView   = nullptr;

    // A texture to associate to the depth stencil view.
    ID3D11Texture2D*            mD3DDepthStencilBuffer = nullptr;

    // Define the functionality of the depth/stencil stages.
    ID3D11DepthStencilState*    mD3DDepthStencilState  = nullptr;

    // Define the functionality of the rasterizer stage.
    ID3D11RasterizerState*      mD3DRasterizerState    = nullptr;

    // Viewport used for the application
    D3D11_VIEWPORT*             mViewport              = nullptr;

};
