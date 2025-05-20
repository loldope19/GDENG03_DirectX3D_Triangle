#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Graphics/Triangle.h> 
#include <vector>

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
        void addTriangle(float posX, float posY, float size, float r, float g, float b, float a = 1.0f);

    private:
        std::shared_ptr<GraphicsDevice> m_graphicsDevice{};
        DeviceContextPtr m_deviceContext{};
        std::unique_ptr<Triangle> m_triangleManager{};
    };
}