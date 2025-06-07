#include <DX3D/Graphics/SwapChain.h>

dx3d::SwapChain::SwapChain(const SwapChainDesc& desc, const GraphicsResourceDesc& gDesc) :
    GraphicsResource(gDesc)
{
    if (!desc.winHandle) DX3DLogThrowInvalidArg("No window handle provided.");

    DXGI_SWAP_CHAIN_DESC dxgiDesc{};

    dxgiDesc.BufferDesc.Width = std::max(1, desc.winSize.width);
    dxgiDesc.BufferDesc.Height = std::max(1, desc.winSize.height);
    dxgiDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiDesc.BufferCount = 2;
    dxgiDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    dxgiDesc.OutputWindow = static_cast<HWND>(desc.winHandle);
    dxgiDesc.SampleDesc.Count = 1;
    dxgiDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    dxgiDesc.Windowed = TRUE;

    DX3DGraphicsLogThrowOnFail(m_factory.CreateSwapChain(&m_device, &dxgiDesc, &m_swapChain),
        "CreateSwapChain failed.");

    reloadBuffers(desc.winSize.width, desc.winSize.height);
}

void dx3d::SwapChain::present(bool vsync)
{
    DX3DGraphicsLogThrowOnFail(m_swapChain->Present(vsync, 0),
        "Present failed.");
}

// Modify reloadBuffers to also create the depth buffer
void dx3d::SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer{};
    DX3DGraphicsLogThrowOnFail(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&buffer)),
        "GetBuffer failed.");
    DX3DGraphicsLogThrowOnFail(m_device.CreateRenderTargetView(buffer.Get(), nullptr, &m_rtv),
        "CreateRenderTargetView failed.");

    // --- Create Depth Stencil Buffer and View ---
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Standard depth format
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    DX3DGraphicsLogThrowOnFail(
        m_device.CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer),
        "Failed to create depth stencil buffer."
    );

    DX3DGraphicsLogThrowOnFail(
        m_device.CreateDepthStencilView(m_depthStencilBuffer.Get(), NULL, &m_dsv),
        "Failed to create depth stencil view."
    );
}