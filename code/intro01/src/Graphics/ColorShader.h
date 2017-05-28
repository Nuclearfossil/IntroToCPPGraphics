#pragma once
class ShaderResource;

class ColorShader
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

public:
    ColorShader();
    ~ColorShader();

    bool InitShader(ID3D11Device* _device, HWND _hwnd, ShaderResource* _vertexShader, ShaderResource* _pixelShader);
    void Shutdown();

    bool Render(ID3D11DeviceContext* _context, DirectX::XMMATRIX& _worldMatrix, DirectX::XMMATRIX& _viewMatrix, DirectX::XMMATRIX& _projectionMatrix);

private:
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext* _context, DirectX::XMMATRIX& _worldMatrix, DirectX::XMMATRIX& _viewMatrix, DirectX::XMMATRIX& _projectionMatrix);
    void RenderShader(ID3D11DeviceContext* _context);

private:
    ID3D11VertexShader*		m_vertexShader;
    ID3D11PixelShader*		m_pixelShader;
    ID3D11InputLayout*		m_layout;
    ID3D11Buffer*			m_matrixBuffer;
};

