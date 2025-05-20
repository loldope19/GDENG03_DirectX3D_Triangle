#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Core/Common.h>
#include <DX3D/Core/Base.h>
#include <d3d11.h>
#include <wrl.h>

namespace dx3d
{
    class GraphicsDevice final : public Base, public std::enable_shared_from_this<GraphicsDevice>
    {
    public:
        explicit GraphicsDevice(const GraphicsDeviceDesc& desc);
        virtual ~GraphicsDevice() override;

        SwapChainPtr createSwapChain(const SwapChainDesc& desc);
        DeviceContextPtr createDeviceContext();

        void executeCommandList(DeviceContext& context);

    private:
        GraphicsResourceDesc getGraphicsResourceDesc() const noexcept;

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice{};
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext{};
        Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgiDevice{};
        Microsoft::WRL::ComPtr<IDXGIAdapter> m_dxgiAdapter{};
        Microsoft::WRL::ComPtr<IDXGIFactory> m_dxgiFactory{};
    };
}
