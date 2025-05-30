#pragma once
#include <stdexcept>
#include <memory>

namespace dx3d {
	class Base;
	class Window;
	class Game;

	class GraphicsEngine;
	class GraphicsDevice;

	class Logger;
	class SwapChain;
	class Display;

	class DeviceContext;

	class ShaderBinary;
	class GraphicsPipelineState;

	using i32 = int;
	using ui32 = unsigned int;
	using f32 = float;
	using d64 = double;

	using SwapChainPtr = std::shared_ptr<SwapChain>;
	using DeviceContextPtr = std::shared_ptr<DeviceContext>;

	using ShaderBinaryPtr = std::shared_ptr<ShaderBinary>;
	using GraphicsPipelineStatePtr = std::shared_ptr<GraphicsPipelineState>;
}
