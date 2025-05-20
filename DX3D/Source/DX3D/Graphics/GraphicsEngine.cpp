#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>

using namespace dx3d;

GraphicsEngine::GraphicsEngine(const GraphicsEngineDesc& desc) : Base(desc.base)
{
    m_graphicsDevice = std::make_shared<GraphicsDevice>(GraphicsDeviceDesc{ m_logger });

    auto& device = *m_graphicsDevice;
    m_deviceContext = device.createDeviceContext();

    GraphicsResourceDesc gDesc = { {m_logger}, m_graphicsDevice,
                                *m_graphicsDevice->m_d3dDevice.Get(),
                                *m_graphicsDevice->m_dxgiFactory.Get() };
    m_triangleManager = std::make_unique<Triangle>(gDesc);

    m_triangleManager->initializeSharedResources();

    addTriangle(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f);
}

GraphicsEngine::~GraphicsEngine()
{
}

GraphicsDevice& GraphicsEngine::getGraphicsDevice() noexcept
{
    return *m_graphicsDevice;
}

void GraphicsEngine::addTriangle(float posX, float posY, float size, float r, float g, float b, float a)
{
    std::vector<TriangleVertex> vertices = {
        { posX,         posY + size / 2, 0.0f, r, g, b, a },  // Top vertex
        { posX + size / 2, posY - size / 2, 0.0f, r, g, b, a },  // Bottom right vertex
        { posX - size / 2, posY - size / 2, 0.0f, r, g, b, a }   // Bottom left vertex
    };

    m_triangleManager->createTriangle(vertices);
}

void GraphicsEngine::render(SwapChain& swapChain)
{
    auto& context = *m_deviceContext;
    context.clearAndSetBackBuffer(swapChain, { 0.f, 0.27f, 0.4f, 1.0f });

    // this thing matches the viewport to the window size
    D3D11_VIEWPORT viewport = {};
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain.m_swapChain->GetDesc(&desc);
    viewport.Width = static_cast<float>(desc.BufferDesc.Width);
    viewport.Height = static_cast<float>(desc.BufferDesc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context.m_context->RSSetViewports(1, &viewport);

    m_triangleManager->render(*context.m_context.Get());

    auto& device = *m_graphicsDevice;
    device.executeCommandList(context);
    swapChain.present();
}