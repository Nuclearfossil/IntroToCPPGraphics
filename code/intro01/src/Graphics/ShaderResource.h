#pragma once

#include <d3dcompiler.h>

class ShaderResource
{
public:
    ShaderResource();
    ~ShaderResource();

    bool LoadShader(const char* filename, const char* shadermodel, const char* entrypoint);
    ID3DBlob* const GetShader() const { return mShaderBuffer; }

private:
    bool CheckHRESULT(HRESULT &hr, ID3DBlob * &errorBlob);
    void OutputShaderErrorMessage(ID3DBlob* _errorMsg, HWND _hwnd, WCHAR* _shaderFilename);

private:
    ID3DBlob* mShaderBuffer;
};
