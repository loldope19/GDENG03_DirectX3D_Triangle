#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Graphics/Triangle.h> 
#include <DX3D/Graphics/Rectangle.h>
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/Camera.h>
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
        void setCamera(Camera* camera) { m_camera = camera; }

        // add a triangle at specified position with specified color
        void addTriangle(float posX, float posY, float size = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        // add a rectangle at specified position with specified size and color
        void addRectangle(float posX, float posY, float width = 1.0f, float height = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        // add a cube at specified position with specified size and color
        void addCube(float posX, float posY, float posZ, float size = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

    private:
        std::shared_ptr<GraphicsDevice> m_graphicsDevice{};
        DeviceContextPtr m_deviceContext{};
        GraphicsPipelineStatePtr m_pipeline{};
        Camera* m_camera{ nullptr };

        std::unique_ptr<Triangle> m_triangleManager{};
        std::unique_ptr<Rectangle> m_rectangleManager{};
        std::unique_ptr<Cube> m_cubeManager{};
    };
}