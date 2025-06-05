#include <DX3D/Graphics/AnimatedRectangle.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>

namespace dx3d
{
    AnimatedRectangle::AnimatedRectangle(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
    {
        m_stride = sizeof(AnimatedRectangleVertex);
        m_offset = 0;
    }

    bool AnimatedRectangle::initializeSharedResources()
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
        if (!m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/AnimatedRectangleVS.hlsl"))
        {
            DX3DLogThrowError("Failed to load animated vertex shader");
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
            DX3DLogThrowError("Failed to load pixel shader for animated rectangle");
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITIONA", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
            { "COLORA",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
            { "POSITIONB", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLORB",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout),
            "Failed to create input layout for animated rectangle"
        );

        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = sizeof(AnimationConstantBufferData);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC; 
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        DX3DGraphicsLogThrowOnFail(
            m_device.CreateBuffer(&cbDesc, nullptr, &m_animationConstantBuffer),
            "Failed to create animation constant buffer"
        );

        m_sharedResourcesInitialized = true;
        return true;
    }

    void AnimatedRectangle::createAnimatedRectangle(const std::vector<AnimatedRectangleVertex>& vertices)
    {
        if (!m_sharedResourcesInitialized && !initializeSharedResources())
        {
            DX3DLogThrowError("Failed to initialize shared resources for animated rectangle");
            return;
        }

        if (vertices.size() != 4)
        {
            DX3DLogThrowError("Animated Rectangle must have exactly 4 vertices");
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
            "Failed to create vertex buffer for animated rectangle"
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
            "Failed to create index buffer for animated rectangle"
        );

        m_vertexBuffers.push_back(vertexBuffer);
        m_indexBuffers.push_back(indexBuffer);
    }

    void AnimatedRectangle::updateAnimation(ID3D11DeviceContext& context, float interpolationFactor)
    {
        if (!m_animationConstantBuffer) return;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        DX3DGraphicsLogThrowOnFail(
            context.Map(m_animationConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
            "Failed to map animation constant buffer"
        );

        AnimationConstantBufferData* dataPtr = static_cast<AnimationConstantBufferData*>(mappedResource.pData);
        dataPtr->interpolationFactor = interpolationFactor;

        context.Unmap(m_animationConstantBuffer.Get(), 0);
    }

    void AnimatedRectangle::render(ID3D11DeviceContext& context)
    {
        if (m_vertexBuffers.empty() || !m_sharedResourcesInitialized)
            return;

        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        context.VSSetConstantBuffers(0, 1, m_animationConstantBuffer.GetAddressOf());

        for (size_t i = 0; i < m_vertexBuffers.size(); i++)
        {
            ID3D11Buffer* vertexBuffers[] = { m_vertexBuffers[i].Get() };
            context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
            context.IASetIndexBuffer(m_indexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
            context.DrawIndexed(6, 0, 0); // 6 indices for 2 triangles forming a rectangle
        }
    }
}