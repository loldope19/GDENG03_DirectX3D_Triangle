#include <DX3D/Game/Game.h>
#include <DX3D/Core/EngineTime.h>
#include <Windows.h>
#include <DX3D/Graphics/GraphicsEngine.h>

void dx3d::Game::run()
{
	MSG msg{};
	while (m_isRunning) 
	{
		EngineTime::logFrameStart();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				m_isRunning = false;
				break;
			}
			if (msg.message == WM_KEYDOWN)
			{
				float currentScale = dx3d::EngineTime::getTimeScale();
				float newScale = currentScale;
				bool scaleChanged = false;
				 
				if (msg.wParam == VK_OEM_PLUS)
				{
				    newScale += 0.5f;
				    scaleChanged = true;
				}
				else if (msg.wParam == VK_OEM_MINUS)
				{
				    newScale -= 0.5f;
				    if (newScale < 0.0f) newScale = 0.0f; // clamp at 0
				    scaleChanged = true;
				}
				else if (msg.wParam == '1')
				{
				    newScale = 1.0f;
				    scaleChanged = true;
				}

				if (msg.wParam == VK_SPACE)
				{
					GraphicsEngine::toggleRotation();
					DX3DLog(getLogger(), Logger::LogLevel::Info, "Space Key Pressed - Rotation Toggled");
				}
				 
				if (scaleChanged)
				{
				    dx3d::EngineTime::setTimeScale(newScale);
				    char buffer[100];
				    sprintf_s(buffer, "Time scale set to: %.2f", newScale);
				    getLogger().log(dx3d::Logger::LogLevel::Info, buffer);
				}
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!m_isRunning) break;

		onInternalUpdate();

		EngineTime::logFrameEnd();
	}
}
