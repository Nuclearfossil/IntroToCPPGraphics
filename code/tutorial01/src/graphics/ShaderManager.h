#pragma once
#include <DirectXMath.h>

///
/// Handy forward declarations
///
class RenderDevice;

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

enum ShaderType
{
    Vertex = 0,
    Pixel,
    Compute,
    Undefined
};

enum PixelShaderLevel
{
    PSM_5_0 = 0,
    PSM_UNDEFINED
};

enum VertexShaderLevel
{
    VSM_5_0 = 0,
    VSM_UNDEFINED
};

// Vertex data for a colored cube.
struct VertexNormalUV
{
    DirectX::XMFLOAT4 Position;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexCoord;
};

// Shader resources
enum ConstanBuffer
{
    CB_Appliation,
    CB_Frame,
    CB_Object,
    NumConstantBuffers
};

class ShaderManager
{
private:
    RenderDevice*           mRenderDevice;
    VertexShaderLevel       mSupportedVertexShaderLevel;
    PixelShaderLevel        mSupportedPixelShaderLevel;

    ID3D11InputLayout*      mD3DInputLayout  = nullptr;
    ID3D11VertexShader*     mD3DVertexShader = nullptr;
    ID3D11PixelShader*      mD3DPixelShader  = nullptr;

    ID3D11Buffer*           mD3DConstantBuffers[NumConstantBuffers];

public:
    ShaderManager();
    ~ShaderManager();

    bool Init(RenderDevice* device);
    void Cleanup();

    HRESULT LoadVSFromFile(LPCWSTR filename, const char* mainfunction);
    HRESULT LoadPSFromFile(LPCWSTR filename, const char* mainfunction);

private:
    void DetermineSupportedShaderLevel(RenderDevice* device);
    const char* CurrentlySupportedShader(ShaderType shaderType);
    ID3D11VertexShader* ShaderManager::CreateVertexShader(ID3DBlob* shaderBlob);
    ID3D11PixelShader* ShaderManager::CreatePixelShader(ID3DBlob* shaderBlob);
};
