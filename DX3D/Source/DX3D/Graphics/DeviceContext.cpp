#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/GraphicsPipelineState.h>

// ... constructor ...
dx3d::DeviceContext::DeviceContext(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
{
	DX3DGraphicsLogThrowOnFail(m_device.CreateDeferredContext(0, &m_context),
		"CreateDeferredContext failed.");
}


void dx3d::DeviceContext::clearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color)
{
	f32 fColor[] = { color.x,color.y,color.z,color.w };
	auto rtv = swapChain.m_rtv.Get();
	auto dsv = swapChain.m_dsv.Get(); // Get the depth stencil view

	// Clear the render target and the depth stencil view
	m_context->ClearRenderTargetView(rtv, fColor);
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Bind the render target and depth stencil view to the pipeline
	m_context->OMSetRenderTargets(1, &rtv, dsv);
}

// ... setGraphicsPipelineState ...
void dx3d::DeviceContext::setGraphicsPipelineState(const GraphicsPipelineState& pipeline)
{
	m_context->VSSetShader(pipeline.m_vs.Get(), nullptr, 0);
	m_context->PSSetShader(pipeline.m_ps.Get(), nullptr, 0);
}