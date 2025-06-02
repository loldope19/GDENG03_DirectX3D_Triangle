#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Shader.h>
#include <DirectXMath.h>

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
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, 3, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout),
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

        // Calculate face normals for each vertex
        std::vector<CubeVertex> verticesWithNormals = vertices;
        
        // Front face normal (0,1,2,3)
        DirectX::XMFLOAT3 frontNormal(0.0f, 0.0f, 1.0f);
        for (int i = 0; i < 4; i++) {
            verticesWithNormals[i].nx = frontNormal.x;
            verticesWithNormals[i].ny = frontNormal.y;
            verticesWithNormals[i].nz = frontNormal.z;
        }

        // Back face normal (4,5,6,7)
        DirectX::XMFLOAT3 backNormal(0.0f, 0.0f, -1.0f);
        for (int i = 4; i < 8; i++) {
            verticesWithNormals[i].nx = backNormal.x;
            verticesWithNormals[i].ny = backNormal.y;
            verticesWithNormals[i].nz = backNormal.z;
        }

        // Top face normal (0,1,5,4)
        DirectX::XMFLOAT3 topNormal(0.0f, 1.0f, 0.0f);
        verticesWithNormals[0].nx = topNormal.x; verticesWithNormals[0].ny = topNormal.y; verticesWithNormals[0].nz = topNormal.z;
        verticesWithNormals[1].nx = topNormal.x; verticesWithNormals[1].ny = topNormal.y; verticesWithNormals[1].nz = topNormal.z;
        verticesWithNormals[5].nx = topNormal.x; verticesWithNormals[5].ny = topNormal.y; verticesWithNormals[5].nz = topNormal.z;
        verticesWithNormals[4].nx = topNormal.x; verticesWithNormals[4].ny = topNormal.y; verticesWithNormals[4].nz = topNormal.z;

        // Bottom face normal (3,2,6,7)
        DirectX::XMFLOAT3 bottomNormal(0.0f, -1.0f, 0.0f);
        verticesWithNormals[3].nx = bottomNormal.x; verticesWithNormals[3].ny = bottomNormal.y; verticesWithNormals[3].nz = bottomNormal.z;
        verticesWithNormals[2].nx = bottomNormal.x; verticesWithNormals[2].ny = bottomNormal.y; verticesWithNormals[2].nz = bottomNormal.z;
        verticesWithNormals[6].nx = bottomNormal.x; verticesWithNormals[6].ny = bottomNormal.y; verticesWithNormals[6].nz = bottomNormal.z;
        verticesWithNormals[7].nx = bottomNormal.x; verticesWithNormals[7].ny = bottomNormal.y; verticesWithNormals[7].nz = bottomNormal.z;

        // Right face normal (1,5,6,2)
        DirectX::XMFLOAT3 rightNormal(1.0f, 0.0f, 0.0f);
        verticesWithNormals[1].nx = rightNormal.x; verticesWithNormals[1].ny = rightNormal.y; verticesWithNormals[1].nz = rightNormal.z;
        verticesWithNormals[5].nx = rightNormal.x; verticesWithNormals[5].ny = rightNormal.y; verticesWithNormals[5].nz = rightNormal.z;
        verticesWithNormals[6].nx = rightNormal.x; verticesWithNormals[6].ny = rightNormal.y; verticesWithNormals[6].nz = rightNormal.z;
        verticesWithNormals[2].nx = rightNormal.x; verticesWithNormals[2].ny = rightNormal.y; verticesWithNormals[2].nz = rightNormal.z;

        // Left face normal (0,4,7,3)
        DirectX::XMFLOAT3 leftNormal(-1.0f, 0.0f, 0.0f);
        verticesWithNormals[0].nx = leftNormal.x; verticesWithNormals[0].ny = leftNormal.y; verticesWithNormals[0].nz = leftNormal.z;
        verticesWithNormals[4].nx = leftNormal.x; verticesWithNormals[4].ny = leftNormal.y; verticesWithNormals[4].nz = leftNormal.z;
        verticesWithNormals[7].nx = leftNormal.x; verticesWithNormals[7].ny = leftNormal.y; verticesWithNormals[7].nz = leftNormal.z;
        verticesWithNormals[3].nx = leftNormal.x; verticesWithNormals[3].ny = leftNormal.y; verticesWithNormals[3].nz = leftNormal.z;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(CubeVertex) * verticesWithNormals.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = verticesWithNormals.data();

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