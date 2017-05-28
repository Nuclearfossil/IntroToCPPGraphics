#include "StdAfx.h"
#include "d3d11.h"
#include "DirectXMath.h"
#include "D3Dcompiler.h"

#include "ColorShader.h"
#include "ShaderResource.h"
#include "Graphics\Mesh.h"
#include "utils\assert.h"

#include <stddef.h>

ColorShader::ColorShader(void)
{
    m_vertexShader = NULL;
    m_pixelShader  = NULL;
    m_layout       = NULL;
    m_matrixBuffer = NULL;
}

ColorShader::~ColorShader()
{
}

void ColorShader::Shutdown()
{
    ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* _context, DirectX::XMMATRIX& _worldMatrix, DirectX::XMMATRIX& _viewMatrix, DirectX::XMMATRIX& _projectionMatrix)
{
    bool result = false;

    result = SetShaderParameters(_context, _worldMatrix, _viewMatrix, _projectionMatrix);
    if (result)
    {
        RenderShader(_context);
        result = true;
    }

    return result;
}

bool ColorShader::InitShader(ID3D11Device* _device, HWND _hwnd, ShaderResource* _vertexShader, ShaderResource* _pixelShader)
{
    ASSERT(_vertexShader != nullptr);
    ASSERT(_pixelShader != nullptr);

    HRESULT result;
    ID3D10Blob* vertexShaderBuffer = _vertexShader->GetShader();
    ID3D10Blob* pixelShaderBuffer = _pixelShader->GetShader();
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Create the vertex shader from the buffer.
    result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if(FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if(FAILED(result))
        return false;

    // Now setup the layout of the data that goes into the shader.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = offsetof(PositionNormalUVLayout, Position);
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "NORMAL";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = offsetof(PositionNormalUVLayout, Normal);
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "TEXCOORD";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = offsetof(PositionNormalUVLayout, UV);
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
        return false;

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = _device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
        return false;

    return true;
}


void ColorShader::RenderShader(ID3D11DeviceContext* _context)
{
    // Set the vertex and pixel shaders that will be used to render this triangle.
    _context->VSSetShader(m_vertexShader, NULL, 0);
    _context->PSSetShader(m_pixelShader, NULL, 0);
}


void ColorShader::ShutdownShader()
{
    // Release the matrix constant buffer.
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // Release the layout.
    if(m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pixel shader.
    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // Release the vertex shader.
    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* _context, DirectX::XMMATRIX& _worldMatrix, DirectX::XMMATRIX& _viewMatrix, DirectX::XMMATRIX& _projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(_worldMatrix);
    viewMatrix = XMMatrixTranspose(_viewMatrix);
    projectionMatrix = XMMatrixTranspose(_projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = _context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
        return false;

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    _context->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    _context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}


