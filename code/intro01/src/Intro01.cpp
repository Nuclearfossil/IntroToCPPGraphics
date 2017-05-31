// CPPD3DIntroduction.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Intro01.h"
#include "AssetManagement\AssetManager.h"
#include "Resource.h"

//==============================================
// TODO:
//   Eventually move this off into a separate utility library
//   We output any caught memory leaks here.
//   See details here: https://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <stdlib.h>
#include "crtdbg.h"
#endif
//==============================================

#include "utils\utils.h"
#include "utils\memory.h"

#include "D3D11.h"
#include "DirectXMath.h"

#include "Graphics\RenderDevice.h"
#include "Graphics\VisualGrid.h"
#include "Graphics\Model.h"
#include "Graphics\ColorShader.h"

#include "Camera.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE _instance, int _cmdShow);
HRESULT InitResources(void);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
RenderDevice    gRenderDevice;
VisualGrid*     gVisualGrid     = nullptr;
Camera*         gCamera         = nullptr;
AssetManager*   gAssetManager   = nullptr;
HINSTANCE       gHInst          = nullptr;
HWND            gHWnd	        = nullptr;


int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                      _In_opt_  HINSTANCE hPrevInstance,
                      _In_      LPWSTR    lpCmdLine,
                      _In_      int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Enable run-time memory check for debug builds.
    // Again, this should live in a separate library
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(InitResources()))
        return 0;

    // Main message loop
    MSG msg = {0};

    Model* model = gAssetManager->GetModel("lte-orb.fbx");
    ShaderResource* psShader = gAssetManager->GetShader("basicPS.hlsl");
    ShaderResource* vsShader = gAssetManager->GetShader("basicVS.hlsl");
    ColorShader colorShader;
    colorShader.InitShader(gRenderDevice.GetDevice(), gHWnd, vsShader, psShader);

    DirectX::XMMATRIX world, view, projection;
    projection  = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians(45.0f), 800.0f/600.0f, 0.1f, 100.0f );

    while (WM_QUIT != msg.message)
    {
        bool test = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (test)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            gCamera->Render();
            view = gCamera->GetViewMatrix();
            projection = gCamera->GetProjMatrix();
            colorShader.Render(gRenderDevice.GetDeviceContext(), world, view, projection);
            model->Render();
            gRenderDevice.Present();
        }
    }

    delete gVisualGrid;
    delete gAssetManager;
    delete gCamera;

#if defined(DEBUG) | defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE _instance, int _cmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.lpszClassName = L"WTGTP_01";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _instance;
    wcex.hIcon = LoadIcon(_instance, (LPCTSTR)IDI_INTRO01);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_INTRO01);

    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    gHInst = _instance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    gHWnd = CreateWindow( L"WTGTP_01", L"Walking The Graphics Pipeline - 01",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top,
                          NULL, NULL,
                          _instance,
                          NULL );

    if (!gHWnd)
        return E_FAIL;

    ShowWindow(gHWnd, _cmdShow);

    if (gRenderDevice.Init(gHWnd, rc.right, rc.bottom, TRUE))
        return S_OK;

    return E_FAIL;
}

HRESULT InitResources( void )
{
    gVisualGrid = gRenderDevice.CreateVisualGrid();

    gCamera = new Camera();
    gCamera->SetPosition(1.0f, 1.0f, 1.0f);


    gAssetManager = new AssetManager();
    gAssetManager->Initialize();
    if (!gAssetManager->AddPath("assets\\raw")) 
        return E_FAIL;
    gAssetManager->LoadModel("lte-orb.fbx");
    gAssetManager->LoadShader("basicPS.hlsl", "ps_5_0", "PSMain");
    gAssetManager->LoadShader("basicVS.hlsl", "vs_5_0", "VSMain");

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
{
    switch(_msg)
    {
    case WM_PAINT:
        gRenderDevice.Present();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SYSCOMMAND:
        switch (_wParam & 0xfff0)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
        case SC_KEYMENU:
            return 0L;
        default:
            break;
        }
        break;

    case WM_SIZE:
        gRenderDevice.ResizeSwapchain(_hWnd);
        break;

    default:
        break;
    }

    return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);;
}
