#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Math/Vec4.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct AnimatedRectangleVertex
    {
        float posX_A, posY_A, posZ_A;
        float r_A, g_A, b_A, a_A;
        float posX_B, posY_B, posZ_B;
        float r_B, g_B, b_B, a_B;
    };

    struct AnimationConstantBufferData
    {
        float interpolationFactor;
        float padding[3];
    };

    class AnimatedRectangle final : public GraphicsResource
    {
    public:
        AnimatedRectangle(const GraphicsResourceDesc& gDesc);

        bool initializeSharedResources();
        void createAnimatedRectangle(const std::vector<AnimatedRectangleVertex>& vertices);
        void updateAnimation(ID3D11DeviceContext& context, float interpolationFactor);
        void render(ID3D11DeviceContext& context);
        size_t getRectangleCount() const { return m_vertexBuffers.size(); }

    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_indexBuffers;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_animationConstantBuffer;

        UINT m_stride;
        UINT m_offset;
        bool m_sharedResourcesInitialized = false;
    };
}