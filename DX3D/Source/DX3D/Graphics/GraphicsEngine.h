#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Graphics/Triangle.h> 
#include <DX3D/Graphics/Rectangle.h>
#include <vector>
#include <chrono>

namespace dx3d
{
    class GraphicsEngine final : public Base
    {
    public:
        explicit GraphicsEngine(const GraphicsEngineDesc& desc);
        virtual ~GraphicsEngine() override;

        GraphicsDevice& getGraphicsDevice() noexcept;

        void render(SwapChain& swapChain);

        // add a triangle at specified position with specified color
        void addTriangle(float posX, float posY, float size = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        // add a rectangle at specified position with specified size and color
        void addRectangle(float posX, float posY, float width = 1.0f, float height = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
        struct AnimationConstants
        {
            UINT time;           // time in milliseconds
            float padding[3];    // padding for 16-byte alignment
        };

        void addAnimatedRectangle(float posX, float posY, float width = 1.0f, float height = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        void createConstantBuffer();
        void updateAnimation();

    private:
        std::shared_ptr<GraphicsDevice> m_graphicsDevice{};
        DeviceContextPtr m_deviceContext{};
        GraphicsPipelineStatePtr m_pipeline{};

        std::unique_ptr<Triangle> m_triangleManager{};
        std::unique_ptr<Rectangle> m_rectangleManager{};
    };
}