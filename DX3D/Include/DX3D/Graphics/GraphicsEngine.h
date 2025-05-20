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
        
        // Add a new triangle to the scene
        void addTriangle(const Vec4& color = Vec4(1.0f, 1.0f, 0.5f, 1.0f), 
                        float x = 0.0f, float y = 0.0f);

    private:
        std::shared_ptr<GraphicsDevice> m_graphicsDevice{};
        DeviceContextPtr m_deviceContext{};
        std::vector<std::unique_ptr<Triangle>> m_triangles{};
    };
} 