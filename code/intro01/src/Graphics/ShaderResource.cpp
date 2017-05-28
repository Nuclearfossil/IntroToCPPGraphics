#include "ShaderResource.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <fstream>

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

void ShaderResource::OutputShaderErrorMessage(ID3DBlob* _errorMsg, HWND _hwnd, WCHAR* _shaderFilename)
{
    char* compileErrors;
    SIZE_T bufferSize, i;
    std::ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(_errorMsg->GetBufferPointer());

    // Get the length of the message.
    bufferSize = _errorMsg->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for(i=0; i<bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    _errorMsg->Release();
    _errorMsg = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(_hwnd, L"Error compiling shader.  Check shader-error.txt for message.", _shaderFilename, MB_OK);
}
