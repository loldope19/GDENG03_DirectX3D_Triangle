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

        bool initializeSharedResources();                                       // sets up all shaders (only once)
        void createTriangle(const std::vector<TriangleVertex>& vertices);       // creates all triangles and subjects them to buffer hell
        void render(ID3D11DeviceContext& context);                              // renders all triangles
        void renderTriangle(ID3D11DeviceContext& context, size_t index);        // i don't think i used this tbh
        size_t getTriangleCount() const { return m_vertexBuffers.size(); }      // gets how many triangles there is

    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        UINT m_stride;
        UINT m_offset;

        bool m_sharedResourcesInitialized = false;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    public:
        struct AnimationData
        {
            float time;
            float offsetX, offsetY;
            float scale;
            float colorR, colorG, colorB, colorA;
            float padding[2]; // Align to 16 bytes
        };

        bool createConstantBuffer();
        void updateAnimation(ID3D11DeviceContext& context, const AnimationData& data);
    };
}
