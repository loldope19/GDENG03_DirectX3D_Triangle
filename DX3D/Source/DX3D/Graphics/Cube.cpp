#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/GraphicsEngine.h>

namespace dx3d
{
    Cube::Cube(const GraphicsResourceDesc& gDesc, const Vec3& rotationAxis, float rotationSpeed)
        : GraphicsResource(gDesc),
        m_rotationAxis(rotationAxis),
        m_rotationSpeed(rotationSpeed),
        m_accumulatedAngle(0.0f)
    {
        m_stride = sizeof(CubeVertex);
        m_offset = 0;
        m_scale = { 1.0f, 1.0f, 1.0f };
        m_accumulatedAngle = 0;

        std::vector<CubeVertex> vertices = {
            // Front face
            {-0.5f, -0.5f, -0.5f, 1,1,0,1}, 
            {-0.5f,  0.5f, -0.5f, 0,0.9f,1,1}, 
            { 0.5f,  0.5f, -0.5f, 1,0,0,1}, 
            { 0.5f, -0.5f, -0.5f, 0,1,0,1},
            // Back face
            { 0.5f, -0.5f,  0.5f, 0,1,0,1}, 
            { 0.5f,  0.5f,  0.5f, 1,0,0,1}, 
            {-0.5f,  0.5f,  0.5f, 0,0.9f,1,1}, 
            {-0.5f, -0.5f,  0.5f, 1,1,0,1},
        };

        std::vector<UINT> indices = {
            // front face
            0,1,2, 0,2,3,       // check
            // back face
            4,5,6, 4,6,7,       // check
            // top face
            1,5,2, 1,6,5,       // check
            // bottom face
            0,3,4, 0,4,7,       // check
            // right face
            3,2,5, 3,5,4,       // check
            // left face
            7,6,1, 7,1,0,       // check
        };

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(CubeVertex) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), 
            "Failed to create cube vertex buffer");

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexInitData = {};
        indexInitData.pSysMem = indices.data();
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &m_indexBuffer), 
            "Failed to create cube index buffer");

        D3D11_BUFFER_DESC constBufferDesc = {};
        constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        constBufferDesc.ByteWidth = sizeof(CB_Cube_Data);
        constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&constBufferDesc, nullptr, &m_constantBuffer), 
            "Failed to create cube constant buffer");

        Shader::ShaderDesc vertexShaderDesc = { 
            { m_logger, m_graphicsDevice, m_device, m_factory }, 
            Shader::Type::Vertex,
            "main", 
            "vs_5_0" };
        m_vertexShader = std::make_unique<Shader>(vertexShaderDesc);
        m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/3DVertexShader.hlsl");

        Shader::ShaderDesc pixelShaderDesc = { 
            { m_logger, m_graphicsDevice, m_device, m_factory }, 
            Shader::Type::Pixel, 
            "main", 
            "ps_5_0" };
        m_pixelShader = std::make_unique<Shader>(pixelShaderDesc);
        m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl");

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, 
            m_vertexShader->getByteCode().data(), 
            m_vertexShader->getByteCode().size(), 
            &m_inputLayout), 
            "Failed to create cube input layout");

        updateWorldMatrix();
    }

    void Cube::setPosition(const Vec3& pos) { m_position = pos; updateWorldMatrix(); }
    void Cube::setRotation(const Vec3& rot) { }
    void Cube::setScale(const Vec3& scale) { m_scale = scale; updateWorldMatrix(); }

    void Cube::updateWorldMatrix()
    {
        Matrix4x4 scaleMatrix, rotationMatrix, translationMatrix;

        scaleMatrix.setScale(m_scale);
        rotationMatrix.setRotation(m_rotationAxis, m_accumulatedAngle);
        translationMatrix.setTranslation(m_position);

        m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
    }

    void Cube::update(float dt)
    {
        if (GraphicsEngine::isRotationEnabled())
        {
            // Update the rotation angle based on speed and time
            m_accumulatedAngle += m_rotationSpeed * dt;
            updateWorldMatrix();
        }
    }

    void Cube::render(ID3D11DeviceContext& context, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix)
    {
        // --- Update constant buffer ---
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context.Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CB_Cube_Data* dataPtr = static_cast<CB_Cube_Data*>(mappedResource.pData);

        // Create copies and transpose them for HLSL
        Matrix4x4 world = m_worldMatrix;
        world.transpose();

        Matrix4x4 view = viewMatrix;
        view.transpose();

        Matrix4x4 projection = projectionMatrix;
        projection.transpose();

        // Update buffer with transposed matrices
        dataPtr->worldMatrix = world;
        dataPtr->viewMatrix = view;
        dataPtr->projectionMatrix = projection;

        context.Unmap(m_constantBuffer.Get(), 0);

        // --- Set resources and draw ---
        context.IASetInputLayout(m_inputLayout.Get());
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        context.VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

        ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
        context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
        context.IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        context.DrawIndexed(36, 0, 0);
    }
}