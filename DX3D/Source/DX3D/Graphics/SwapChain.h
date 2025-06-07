#pragma once
#include <DX3D/Graphics/GraphicsResource.h>

namespace dx3d
{
	class SwapChain final : public GraphicsResource
	{
	public:
		SwapChain(const SwapChainDesc& desc, const GraphicsResourceDesc& gDesc);

		void present(bool vsync = false);

	private:
		void reloadBuffers(unsigned int width, unsigned int height); // Update signature

	public:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain{};
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv{};
		// --- Add these ---
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv{};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer{};

		friend class DeviceContext;
	};
}