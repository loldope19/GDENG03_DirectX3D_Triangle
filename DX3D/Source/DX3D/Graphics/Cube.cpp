#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Shader.h>

namespace dx3d
{
    Cube::Cube(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
    {
        m_stride = sizeof(CubeVertex);
        m_offset = 0;
    }

    bool Cube::initializeSharedResources()
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
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout),
            "Failed to create input layout"
        );

        m_sharedResourcesInitialized = true;
        return true;
    }

    void Cube::createCube(const std::vector<CubeVertex>& vertices)
    {
        if (!m_sharedResourcesInitialized && !initializeSharedResources())
        {
            DX3DLogThrowError("Failed to initialize shared resources");
            return;
        }

        if (vertices.size() != 8)
        {
            DX3DLogThrowError("Cube must have exactly 8 vertices");
            return;
        }

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(CubeVertex) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&bufferDesc, &initData, &vertexBuffer),
            "Failed to create vertex buffer"
        );

        // Define indices for the cube (6 faces, 2 triangles per face, 3 vertices per triangle)
        UINT indices[] = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 6, 5, 4, 7, 6,
            // Top face
            4, 5, 1, 4, 1, 0,
            // Bottom face
            3, 2, 6, 3, 6, 7,
            // Right face
            1, 5, 6, 1, 6, 2,
            // Left face
            4, 0, 3, 4, 3, 7
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

    void Cube::render(ID3D11DeviceContext& context)
    {
        if (m_vertexBuffers.empty() || !m_sharedResourcesInitialized)
            return;

        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        // render all cubes
        for (size_t i = 0; i < m_vertexBuffers.size(); i++)
        {
            ID3D11Buffer* vertexBuffers[] = { m_vertexBuffers[i].Get() };
            context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
            context.IASetIndexBuffer(m_indexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
            context.DrawIndexed(36, 0, 0); // 36 indices for 12 triangles (6 faces * 2 triangles * 3 vertices)
        }
    }
} 