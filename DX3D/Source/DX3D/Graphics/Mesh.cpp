#include <DX3D/Graphics/Mesh.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>

namespace dx3d
{
    Mesh::Mesh(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
    {
        initializeShaders();
    }

    void Mesh::initializeBuffers(const std::vector<Vertex>& vertices)
    {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer),
            "Failed to create vertex buffer"
        );

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout),
            "Failed to create input layout"
        );

        m_stride = sizeof(Vertex);
        m_offset = 0;
        m_vertexCount = static_cast<UINT>(vertices.size());
    }

    void Mesh::initializeShaders()
    {
        Shader::ShaderDesc vertexShaderDesc = {
            { m_logger, m_graphicsDevice, m_device, m_factory },
            Shader::Type::Vertex,
            "main",
            "vs_5_0"
        };
        m_vertexShader = std::make_unique<Shader>(vertexShaderDesc);
        if (!m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/VertexShader.hlsl"))
        {
            DX3DLogThrowError("Failed to load vertex shader");
        }

        Shader::ShaderDesc pixelShaderDesc = {
            { m_logger, m_graphicsDevice, m_device, m_factory },
            Shader::Type::Pixel,
            "main",
            "ps_5_0"
        };
        m_pixelShader = std::make_unique<Shader>(pixelShaderDesc);
        if (!m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl"))
        {
            DX3DLogThrowError("Failed to load pixel shader");
        }
    }

    void Mesh::render(ID3D11DeviceContext& context)
    {
        context.IASetInputLayout(m_inputLayout.Get());

        ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
        context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);

        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        context.Draw(m_vertexCount, 0);
    }
} 