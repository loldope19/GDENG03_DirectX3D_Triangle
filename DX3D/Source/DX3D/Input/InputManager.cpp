#include <DX3D/Input/InputManager.h>

namespace dx3d
{
    InputManager::InputManager()
    {
        // Capture initial mouse position on construction
        GetCursorPos(&m_prevMousePos);
        m_initialized = true;
    }

    void InputManager::update()
    {
        POINT currentPos;
        GetCursorPos(&currentPos);

        if (m_initialized)
        {
            m_deltaX = static_cast<float>(currentPos.x - m_prevMousePos.x);
            m_deltaY = static_cast<float>(currentPos.y - m_prevMousePos.y);
        }

        m_prevMousePos = currentPos;
    }

    bool InputManager::isKeyDown(int key) const
    {
        return (GetAsyncKeyState(key) & 0x8000) != 0;
    }

    float InputManager::getMouseDeltaX() const
    {
        return m_deltaX;
    }

    float InputManager::getMouseDeltaY() const
    {
        return m_deltaY;
    }
}