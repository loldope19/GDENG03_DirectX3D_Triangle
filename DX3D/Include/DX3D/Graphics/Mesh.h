#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct Vertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    class Mesh : public GraphicsResource
    {
    public:
        Mesh(const GraphicsResourceDesc& gDesc);
        virtual ~Mesh() = default;

        void render(ID3D11DeviceContext& context);

    protected:
        void initializeBuffers(const std::vector<Vertex>& vertices);
        void initializeShaders();

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        UINT m_stride;
        UINT m_offset;
        UINT m_vertexCount;
    };
} 