#include "ShaderResource.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


ShaderResource::ShaderResource()
{
    mShaderBuffer = nullptr;
}


ShaderResource::~ShaderResource()
{
}

bool ShaderResource::LoadShader(const char* filename, const char* shadermodel, const char* entrypoint)
{
    bool result = false;
    wchar_t buffer[256];
    mbstowcs(buffer, filename, 255);
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(buffer, 0,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    entrypoint, shadermodel,
                                    (D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG),
                                    0,
                                    &mShaderBuffer, &errorBlob);

    if (CheckHRESULT(hr, errorBlob)) result = true;

    return result;
}

bool ShaderResource::CheckHRESULT(HRESULT &hr, ID3DBlob * &errorBlob)
{
    bool result = true;
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
            errorBlob = nullptr;
        }
        result = false;
    }
    return result;
}
