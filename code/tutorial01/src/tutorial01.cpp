///--------------------------------------------------------------------------------------
/// CPPD3DIntroduction.cpp : Defines the entry point for the application.
///--------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Resource.h"

///--------------------------------------------------------------------------------------
/// TODO:
///   Eventually move this off into a separate utility library
///   We output any caught memory leaks here.
///   See details here: https://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
#if defined(DEBUG) || defined(_DEBUG)
#include <stdlib.h>
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <crtdbg.h>
#endif
///--------------------------------------------------------------------------------------

///--------------------------------------------------------------------------------------
/// General needs includes
///--------------------------------------------------------------------------------------
#include <timeapi.h>
#include <algorithm>

///--------------------------------------------------------------------------------------
/// A little temporary include to use DirectX::Colors
///--------------------------------------------------------------------------------------
#include <DirectXColors.h>

///--------------------------------------------------------------------------------------
/// Rendering includes
///--------------------------------------------------------------------------------------
#include "graphics\RenderDevice.h"
#include "graphics\ShaderManager.h"

///--------------------------------------------------------------------------------------
/// Utilities
///--------------------------------------------------------------------------------------
#include "utils\utils.h"
#include "utils\memory.h"

///--------------------------------------------------------------------------------------
/// Forward declarations
///--------------------------------------------------------------------------------------
HRESULT InitWindow(int cmdShow);
HRESULT InitResources();
void Shutdown();

void Render();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

///--------------------------------------------------------------------------------------
/// Globals
///--------------------------------------------------------------------------------------
HINSTANCE       gHInst          = nullptr;
HWND            gHWnd           = nullptr;
RenderDevice*   gRenderDevice   = nullptr;
ShaderManager*  gShaderManager  = nullptr;


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
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
                   _CRTDBG_LEAK_CHECK_DF);
#endif
    gHInst = hInstance;

    if (FAILED(InitWindow(nCmdShow)))
        return 0;

    if (FAILED(InitResources()))
        return 0;

    // Main message loop
    MSG msg = {0};

    static DWORD previousTime = timeGetTime();
    static const float targetFramerate = 30.0f;
    static const float maxTimeStep = 1.0f / targetFramerate;

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

            Render();
        }
    }

    Shutdown();

#if defined(DEBUG) | defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return (int)msg.wParam;
}

///--------------------------------------------------------------------------------------
/// Register class and create window
///--------------------------------------------------------------------------------------
HRESULT InitWindow(int cmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.lpszClassName = L"TUTORIAL_01";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = gHInst;
    wcex.hIcon = LoadIcon(gHInst, (LPCTSTR)IDI_TUTORIAL01);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TUTORIAL01);
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL01);

    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    gHWnd = CreateWindow(L"TUTORIAL_01",
                         L"Tutorial - 01",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         rc.right - rc.left, rc.bottom - rc.top,
                         NULL, NULL,
                         gHInst,
                         NULL);

    if (!gHWnd)
        return E_FAIL;

    ShowWindow(gHWnd, cmdShow);

    return S_OK;
}

///--------------------------------------------------------------------------------------
/// Initialize all resources.
///--------------------------------------------------------------------------------------
HRESULT InitResources()
{
    gRenderDevice = new RenderDevice();
    if (gRenderDevice->InitDevice(gHWnd, false) != 0)
        return E_FAIL;

    gShaderManager = new ShaderManager();
    if (!gShaderManager->Init(gRenderDevice))
        return E_FAIL;

    if (FAILED(gShaderManager->LoadVSFromFile(L".\\assets\\raw\\basicVS.hlsl", "VSMain")))
        return E_FAIL;

    if (FAILED(gShaderManager->LoadPSFromFile(L".\\assets\\raw\\basicPS.hlsl", "PSMain")))
        return E_FAIL;

    return S_OK;
}

///--------------------------------------------------------------------------------------
/// clean up after ourselves
///--------------------------------------------------------------------------------------
void Shutdown()
{
    gShaderManager->Cleanup();
    delete gShaderManager;
    gShaderManager = nullptr;

    delete gRenderDevice;
    gRenderDevice = nullptr;
}

///--------------------------------------------------------------------------------------
/// Called every time the application receives a message
///--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        // Will need to revisit this.
        PAINTSTRUCT paintStruct;
        BeginPaint(hWnd, &paintStruct);
        EndPaint(hWnd, &paintStruct);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_ABOUT:
            MessageBox(hWnd, L"Simple Application for DX11", L"About", MB_OK);
            break;
        default:
            break;
        }
        break;

    case WM_SYSCOMMAND:
        switch (wParam & 0xfff0)
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
        break;

    default:
        break;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);;
}


void Render()
{
    assert(gRenderDevice);

    gRenderDevice->Clear(DirectX::Colors::CornflowerBlue, 1.0f, 0);

    gRenderDevice->Present(false);
}