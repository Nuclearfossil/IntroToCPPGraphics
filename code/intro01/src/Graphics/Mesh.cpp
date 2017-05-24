#include "StdAfx.h"
#include "Mesh.h"
#include "utils\utils.h"

#include <d3d11.h>

Mesh::Mesh()
{
    mIndexBufferData = nullptr;
    mRawVertexData = nullptr;
    mIndexBuffer = nullptr;
}

Mesh::~Mesh()
{
    if (mIndexBuffer != nullptr)
    {
        delete mIndexBuffer;
        mIndexBuffer = nullptr;
    }

    delete[] mRawVertexData;
    delete[] mIndexBufferData;
}

bool Mesh::Load(PositionNormalUVLayout* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount)
{
    mRawVertexData = vertices;
    mIndexCount = indexCount;
    mIndexBufferData = indices;

    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA resourceData;

    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(PositionNormalUVLayout) * vertexCount;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    resourceData.pSysMem = vertices;
    return false;
}