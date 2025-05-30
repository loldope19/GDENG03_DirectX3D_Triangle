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
            DX3DLogThrowError("Failed to load vertex shader");
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
            DX3DLogThrowError("Failed to load pixel shader");
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, 4, m_vertexShader->getByteCode().data(),
                m_vertexShader->getByteCode().size(), &m_inputLayout),
            "Failed to create input layout"
        );

        if (!createConstantBuffer())
        {
            DX3DLogThrowError("Failed to create constant buffer");
            return false;
        }

        m_sharedResourcesInitialized = true;
        return true;
    }

    void Rectangle::createRectangle(const std::vector<RectangleVertex>& vertices)
    {
        if (!m_sharedResourcesInitialized && !initializeSharedResources())
        {
            DX3DLogThrowError("Failed to initialize shared resources");
            return;
        }
        
        if (vertices.size() != 4)
        {
            DX3DLogThrowError("Rectangle must have exactly 4 vertices");
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

    void Rectangle::createAnimatedRectangle(const std::vector<AnimatedRectangleVertex>& vertices)
    {
        if (!m_sharedResourcesInitialized && !initializeSharedResources())
        {
            DX3DLogThrowError("Failed to initialize shared resources");
            return;
        }

        if (vertices.size() != 4)
        {
            DX3DLogThrowError("Rectangle must have exactly 4 vertices");
            return;
        }

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(AnimatedRectangleVertex) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&bufferDesc, &initData, &vertexBuffer),
            "Failed to create vertex buffer"
        );

        UINT indices[] = {
            0, 1, 2,
            0, 2, 3
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

    bool Rectangle::createConstantBuffer()
    {
        const size_t bufferSize = (sizeof(AnimationData) + 15) & ~15;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = static_cast<UINT>(bufferSize);
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        HRESULT hr = m_device.CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
        if (FAILED(hr))
        {
            // DX3DLogThrowError("Failed to create constant buffer");
            return false;
        }
        return true;
    }

    void Rectangle::updateAnimation(ID3D11DeviceContext& context, const AnimationData& data)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context.Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, &data, sizeof(AnimationData));
            context.Unmap(m_constantBuffer.Get(), 0);
        }

        ID3D11Buffer* buffers[] = { m_constantBuffer.Get() };
        context.VSSetConstantBuffers(0, 1, buffers);
    }

    void Rectangle::render(ID3D11DeviceContext& context)
    {
        if (m_vertexBuffers.empty() || !m_sharedResourcesInitialized)
            return;

        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        // Render all rectangles
        for (size_t i = 0; i < m_vertexBuffers.size(); i++)
        {
            ID3D11Buffer* vertexBuffers[] = { m_vertexBuffers[i].Get() };
            context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
            context.IASetIndexBuffer(m_indexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
            context.DrawIndexed(6, 0, 0);  // 6 indices for two triangles
        }
    }
}