// CPPD3DIntroduction.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Intro01.h"
#include "AssetManagement\AssetManager.h"
#include "Resource.h"

#include "D3D11.h"
#include "Graphics\RenderDevice.h"
#include "Graphics\VisualGrid.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE _instance, int _cmdShow );
HRESULT InitResources( void );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
RenderDevice    gRenderDevice;
VisualGrid*     gVisualGrid = NULL;
AssetManager    gAssetManager;
HINSTANCE	    gHInst = NULL;
HWND		    gHWnd	= NULL;


int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                     _In_opt_   HINSTANCE hPrevInstance,
                     _In_       LPWSTR    lpCmdLine,
                     _In_       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
    {
        return 0;
    }

    if ( FAILED(InitResources()) )
    {
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            gRenderDevice.Present();
        }
    }

    delete gVisualGrid;

    return ( int )msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE _instance, int _cmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _instance;
    wcex.hIcon = LoadIcon( _instance, ( LPCTSTR )IDI_INTRO01 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"WTGTP_01";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_INTRO01 );
    if( !RegisterClassEx( &wcex ) )
    {
        return E_FAIL;
    }

    // Create window
    gHInst = _instance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    gHWnd = CreateWindow( L"WTGTP_01", L"Walking The Graphics Pipeline - 01", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, _instance,
        NULL );
    if( !gHWnd )
    {
        return E_FAIL;
    }

    ShowWindow( gHWnd, _cmdShow );

    if( gRenderDevice.Init( gHWnd, rc.right, rc.bottom, TRUE ) )
    {
        return S_OK;
    }

    return S_FALSE;
}

HRESULT InitResources( void )
{
    gVisualGrid = gRenderDevice.CreateVisualGrid();

    gAssetManager.Initialize();

    if (!gAssetManager.AddPath("assets\\raw"))
    {
        return S_FALSE;
    }

    gAssetManager.LoadMesh("lte-orb.fbx");

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
{
    switch( _msg )
    {
    case WM_PAINT:
        gRenderDevice.Present();
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_SIZE:
        // TODO: Only update the size when we're finished resizing
        gRenderDevice.ResizeSwapchain(_hWnd);
        break;

    default:
        return DefWindowProc( _hWnd, _msg, _wParam, _lParam );
    }

    return 0;
}

