#include <DX3D/Graphics/Rectangle.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Shader.h>

namespace dx3d
{
    Rectangle::Rectangle(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
    {
        m_stride = sizeof(RectangleVertex);
        m_offset = 0;
    }

    bool Rectangle::initializeSharedResources()
    {
        if (m_sharedResourcesInitialized)
            return true;

        Shader::ShaderDesc vertexShaderDesc = {
            { m_logger, m_graphicsDevice, m_device, m_factory },
            Shader::Type::Vertex,
            "main",
            "vs_5_0"
        };
        m_vertexShader = std::make_unique<Shader>(vertexShaderDesc);
        if (!m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/VertexShader.hlsl"))
        {
            DX3DLogErrorAndThrow("Failed to load vertex shader");
            return false;
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
            DX3DLogErrorAndThrow("Failed to load pixel shader");
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout),
            "Failed to create input layout"
        );

        m_sharedResourcesInitialized = true;
        return true;
    }

    void Rectangle::createRectangle(const std::vector<RectangleVertex>& vertices)
    {
        if (!m_sharedResourcesInitialized && !initializeSharedResources())
        {
            DX3DLogErrorAndThrow("Failed to initialize shared resources");
            return;
        }
        
        if (vertices.size() != 4)
        {
            DX3DLogErrorAndThrow("Rectangle must have exactly 4 vertices");
            return;
        }

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(RectangleVertex) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&bufferDesc, &initData, &vertexBuffer),
            "Failed to create vertex buffer"
        );

        // two triangles to form a rectangle
        UINT indices[] = {
            0, 1, 2,  // First triangle
            0, 2, 3   // Second triangle
        };

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(indices);
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexInitData = {};
        indexInitData.pSysMem = indices;

        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &indexBuffer),
            "Failed to create index buffer"
        );

        m_vertexBuffers.push_back(vertexBuffer);
        m_indexBuffers.push_back(indexBuffer);
    }

    void Rectangle::render(ID3D11DeviceContext& context)
    {
        if (m_vertexBuffers.empty() || !m_sharedResourcesInitialized)
            return;

        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        // render all rectangles
        for (size_t i = 0; i < m_vertexBuffers.size(); i++)
        {
            ID3D11Buffer* vertexBuffers[] = { m_vertexBuffers[i].Get() };
            context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
            context.IASetIndexBuffer(m_indexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
            context.DrawIndexed(6, 0, 0); // 6 indices for 2 triangles
        }
    }

    void Rectangle::renderRectangle(ID3D11DeviceContext& context, size_t index)
    {
        if (index >= m_vertexBuffers.size() || !m_sharedResourcesInitialized)
            return;

        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        ID3D11Buffer* vertexBuffers[] = { m_vertexBuffers[index].Get() };
        context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
        context.IASetIndexBuffer(m_indexBuffers[index].Get(), DXGI_FORMAT_R32_UINT, 0);
        context.DrawIndexed(6, 0, 0);
    }
}