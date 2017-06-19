#pragma once
class RenderDevice;

class CubeMesh
{
public:
    CubeMesh();
    ~CubeMesh();
    bool Initialize(RenderDevice* device);
    void Draw(RenderDevice* device, ID3D11InputLayout* layout);

private:
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
};
