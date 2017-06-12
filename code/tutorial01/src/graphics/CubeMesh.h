#pragma once
class RenderDevice;

class CubeMesh
{
public:
    CubeMesh();
    ~CubeMesh();
    bool Initialize(RenderDevice* device);

private:
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

};