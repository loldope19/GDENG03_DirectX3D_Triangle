#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Core.h>
#include <DX3D/Graphics/Camera.h>
#include <DX3D/Input/InputManager.h>


namespace dx3d {
	class Game : public Base
	{
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		virtual void run() final;

	private:
		void onInternalUpdate();
		void handleInput();

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GraphicsEngine> m_graphicsEngine{};
		std::unique_ptr<Display> m_display{};

		std::unique_ptr<Camera> m_camera{};
		std::unique_ptr<InputManager> m_inputManager{};

		bool m_isRunning{ true };
	};

}

