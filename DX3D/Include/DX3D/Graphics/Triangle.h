#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct TriangleVertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    class Triangle : public GraphicsResource
    {
    public:
        Triangle(const GraphicsResourceDesc& gDesc);

        bool initializeSharedResources();

        void createTriangle(const std::vector<TriangleVertex>& vertices);

        void render(ID3D11DeviceContext& context);

        void renderTriangle(ID3D11DeviceContext& context, size_t index);

        size_t getTriangleCount() const { return m_vertexBuffers.size(); }

    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        UINT m_stride;
        UINT m_offset;

        // Flag to track if shared resources are initialized
        bool m_sharedResourcesInitialized = false;
    };
}
