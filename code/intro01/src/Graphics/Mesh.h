#pragma once

#include "AssetManagement\IResource.h"

#include <DirectXMath.h>

using namespace DirectX;

// ======================================================================================
// Forward Declarations - so the header file doesn't have to #include anything
// ======================================================================================
class ID3D11Buffer;

// Initial Mesh Layout - Consists of a Postion, Normal and Single Texture UV
struct PositionNormalUVLayout
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
    XMFLOAT2 UV;
};

class Mesh : public IResource
{
public:
    Mesh();
    virtual ~Mesh() override;

    bool Load(PositionNormalUVLayout* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);

    void Render();

private:
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    PositionNormalUVLayout* mRawVertexData;
    unsigned int* mIndexBufferData;
    unsigned int mIndexCount;
};

