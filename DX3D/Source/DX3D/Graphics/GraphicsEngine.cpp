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

    // Create constant buffer for animation
    createConstantBuffer();

    constexpr char shaderSourceCode[] =
        R"(
void VSMain()
{
}
void PSMain()
{
}
)";
    constexpr char shaderSourceName[] = "Basic";
    constexpr auto shaderSourceCodeSize = std::size(shaderSourceCode);

    auto vs = device.compileShader({ shaderSourceName, shaderSourceCode, shaderSourceCodeSize,
        "VSMain", ShaderType::VertexShader });
    auto ps = device.compileShader({ shaderSourceName, shaderSourceCode, shaderSourceCodeSize,
        "PSMain", ShaderType::PixelShader });

    m_pipeline = device.createGraphicsPipelineState({ *vs,*ps });

    GraphicsResourceDesc gDesc = { {m_logger}, m_graphicsDevice,
                                *m_graphicsDevice->m_d3dDevice.Get(),
                                *m_graphicsDevice->m_dxgiFactory.Get() };
    m_triangleManager = std::make_unique<Triangle>(gDesc);
    m_rectangleManager = std::make_unique<Rectangle>(gDesc);

    m_triangleManager->initializeSharedResources();
    m_rectangleManager->initializeSharedResources();

    addAnimatedRectangle(0.4f, -0.2f);
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
    std::vector<TriangleVertex> vertices;
    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            { posX,         posY + size / 2, 0.0f, 1.0f, 0.0f, 0.0f, a },  // Top: Red
            { posX + size / 2, posY - size / 2, 0.0f, 0.0f, 1.0f, 0.0f, a },  // Bottom right: Green
            { posX - size / 2, posY - size / 2, 0.0f, 0.0f, 0.0f, 1.0f, a }   // Bottom left: Blue
        };
    }
    else {
        vertices = {
            { posX,         posY + size / 2, 0.0f, r, g, b, a },  // Top vertex
            { posX + size / 2, posY - size / 2, 0.0f, r, g, b, a },  // Bottom right vertex
            { posX - size / 2, posY - size / 2, 0.0f, r, g, b, a }   // Bottom left vertex
        };
    }

    m_triangleManager->createTriangle(vertices);
}

void dx3d::GraphicsEngine::addRectangle(float posX, float posY, float width, float height, float r, float g, float b, float a)
{
    std::vector<RectangleVertex> vertices;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            { posX - halfWidth, posY + halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, a },  // Top-left: Green
            { posX + halfWidth, posY + halfHeight, 0.0f, 1.0f, 1.0f, 0.0f, a },  // Top-right: Yellow
            { posX + halfWidth, posY - halfHeight, 0.0f, 0.0f, 0.0f, 1.0f, a },  // Bottom-right: Blue
            { posX - halfWidth, posY - halfHeight, 0.0f, 1.0f, 0.0f, 0.0f, a }   // Bottom-left: Red
        };
    }
    else {
        vertices = {
            { posX - halfWidth, posY + halfHeight, 0.0f, r, g, b, a },  // Top-left
            { posX + halfWidth, posY + halfHeight, 0.0f, r, g, b, a },  // Top-right
            { posX + halfWidth, posY - halfHeight, 0.0f, r, g, b, a },  // Bottom-right
            { posX - halfWidth, posY - halfHeight, 0.0f, r, g, b, a }   // Bottom-left
        };
    }

    m_rectangleManager->createRectangle(vertices);
}

void GraphicsEngine::addAnimatedRectangle(float posX, float posY, float width, float height, float r, float g, float b, float a)
{
    std::vector<AnimatedRectangleVertex> vertices;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            // Top-left: Red
            { posX - halfWidth, posY + halfHeight, 0.0f,
              posX - halfWidth + 0.2f, posY + halfHeight + 0.2f, 0.0f,
              1.0f, 0.0f, 0.0f, a, 
              0.0f, 1.0f, 0.0f, a },

            // Top-right: Yellow
            { posX + halfWidth, posY + halfHeight, 0.0f,
              posX + halfWidth + 0.2f, posY + halfHeight + 0.2f, 0.0f,
              1.0f, 1.0f, 0.0f, a,                                    // color1 (yellow)
              0.0f, 0.0f, 1.0f, a },                                 // color2 (blue)

            // Bottom-right: Cyan
            { posX + halfWidth, posY - halfHeight, 0.0f,
              posX + halfWidth + 0.2f, posY - halfHeight + 0.2f, 0.0f,
              0.0f, 1.0f, 1.0f, a,                                    // color1 (cyan)
              1.0f, 0.0f, 1.0f, a },                                 // color2 (magenta)

            // Bottom-left: Green
            { posX - halfWidth, posY - halfHeight, 0.0f,
              posX - halfWidth + 0.2f, posY - halfHeight + 0.2f, 0.0f,
              0.0f, 1.0f, 0.0f, a,                                    // color1 (green)
              1.0f, 0.5f, 0.0f, a }                                  // color2 (orange)
        };
    }
    else {
        vertices = {
            { posX - halfWidth, posY + halfHeight, 0.0f,     // position1
              posX - halfWidth + 0.2f, posY + halfHeight + 0.1f, 0.0f,  // position2
              r, g, b, a,                                    // color1
              1.0f - r, 1.0f - g, 1.0f - b, a },           // color2 (inverted)

            { posX + halfWidth, posY + halfHeight, 0.0f,
              posX + halfWidth - 0.1f, posY + halfHeight + 0.2f, 0.0f,
              r, g, b, a,
              1.0f - r, 1.0f - g, 1.0f - b, a },

            { posX + halfWidth, posY - halfHeight, 0.0f,
              posX + halfWidth - 0.2f, posY - halfHeight - 0.1f, 0.0f,
              r, g, b, a,
              1.0f - r, 1.0f - g, 1.0f - b, a },

            { posX - halfWidth, posY - halfHeight, 0.0f,
              posX - halfWidth + 0.1f, posY - halfHeight - 0.2f, 0.0f,
              r, g, b, a,
              1.0f - r, 1.0f - g, 1.0f - b, a }
        };
    }

    m_rectangleManager->createAnimatedRectangle(vertices);
}

void GraphicsEngine::createConstantBuffer()
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(AnimationConstants);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_graphicsDevice->m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
    if (FAILED(hr))
    {
        DX3DLogThrowError("Failed to create constant buffer");
    }
}

void GraphicsEngine::updateAnimation()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

    AnimationConstants constants{};
    constants.time = static_cast<UINT>(elapsed.count());

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_deviceContext->m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &constants, sizeof(AnimationConstants));
        m_deviceContext->m_context->Unmap(m_constantBuffer.Get(), 0);
    }
}

void GraphicsEngine::render(SwapChain& swapChain)
{
    auto& context = *m_deviceContext;
    context.clearAndSetBackBuffer(swapChain, { 0.f, 0.27f, 0.4f, 1.0f });

    updateAnimation();

    ID3D11Buffer* constantBuffers[] = { m_constantBuffer.Get() };
    context.m_context->VSSetConstantBuffers(0, 1, constantBuffers);
    context.m_context->PSSetConstantBuffers(0, 1, constantBuffers);

    context.setGraphicsPipelineState(*m_pipeline);

    D3D11_VIEWPORT viewport = {};
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain.m_swapChain->GetDesc(&desc);
    viewport.Width = static_cast<float>(desc.BufferDesc.Width);
    viewport.Height = static_cast<float>(desc.BufferDesc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context.m_context->RSSetViewports(1, &viewport);

    m_triangleManager->render(*context.m_context.Get());
    m_rectangleManager->render(*context.m_context.Get());

    auto& device = *m_graphicsDevice;
    device.executeCommandList(context);
    swapChain.present();
}