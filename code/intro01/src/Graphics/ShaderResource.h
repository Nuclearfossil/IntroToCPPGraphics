#pragma once

#include <d3dcompiler.h>

class ShaderResource
{
public:
    ShaderResource();
    ~ShaderResource();

    bool LoadShader(const char* filename, const char* shadermodel, const char* entrypoint);

private:
    bool CheckHRESULT(HRESULT &hr, ID3DBlob * &errorBlob);

private:
    ID3DBlob* mShaderBuffer;
};
