#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "graphics\RenderDevice.h"
#include "graphics\CubeMesh.h"
#include "graphics\ShaderManager.h"

/// And any utilities
#include "utils\utils.h"


VertexNormalUV gVertices[8] = 
{
    { DirectX::XMFLOAT4( -1.0f, -1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 0
    { DirectX::XMFLOAT4( -1.0f,  1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 1
    { DirectX::XMFLOAT4(  1.0f,  1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 2
    { DirectX::XMFLOAT4(  1.0f, -1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 3
    { DirectX::XMFLOAT4( -1.0f, -1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 4
    { DirectX::XMFLOAT4( -1.0f,  1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 5
    { DirectX::XMFLOAT4(  1.0f,  1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }, // 6
    { DirectX::XMFLOAT4(  1.0f, -1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) }  // 7
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

CubeMesh::CubeMesh()
    : mVertexBuffer(nullptr),
      mIndexBuffer(nullptr)
{}

CubeMesh::~CubeMesh()
{
    SafeRelease(mVertexBuffer);
    SafeRelease(mIndexBuffer);
}

bool CubeMesh::Initialize(RenderDevice* device)
{
    assert(device);

    ID3D11Device* d3dDevice = device->GetD3D11();

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

    HRESULT hr = d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &mVertexBuffer);
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

    hr = d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &mIndexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    SafeRelease(d3dDevice);

    return true;
}

void CubeMesh::Draw(RenderDevice* device, ID3D11InputLayout* layout)
{
    const UINT vertexStride = sizeof(VertexNormalUV);
    const UINT offset = 0;
    ID3D11DeviceContext* context = nullptr;

    device->GetD3D11()->GetImmediateContext(&context);
    context->IASetVertexBuffers(0, 1, &mVertexBuffer, &vertexStride, &offset);
    context->IASetInputLayout(layout);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(_countof(gIndicies), 0, 0);

}