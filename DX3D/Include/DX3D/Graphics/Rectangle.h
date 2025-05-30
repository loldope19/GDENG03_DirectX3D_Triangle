#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct RectangleVertex
    {
        float x, y, z;    // position
        float r, g, b, a; // color
    };

    struct AnimatedRectangleVertex
    {
        float x, y, z;           // position1
        float x2, y2, z2;        // position2
        float r, g, b, a;        // color1
        float r2, g2, b2, a2;    // color2
    };

    class Rectangle : public GraphicsResource
    {
    public:
        Rectangle(const GraphicsResourceDesc& gDesc);

        bool initializeSharedResources();                                       // sets up all shaders (only once)
        void createRectangle(const std::vector<RectangleVertex>& vertices);     // creates rectangle and subjects it to buffer hell
        void createAnimatedRectangle(const std::vector<AnimatedRectangleVertex>& vertices);
        void render(ID3D11DeviceContext& context);                              // renders all rectanglesy
        size_t getRectangleCount() const { return m_vertexBuffers.size(); }     // gets how many rectangles there are

    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_indexBuffers;
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