#pragma once
#include <Windows.h>

namespace dx3d
{
    class InputManager
    {
    public:
        InputManager();
        void update();                          // Call once per frame
        bool isKeyDown(int key) const;

        float getMouseDeltaX() const;
        float getMouseDeltaY() const;

    private:
        POINT m_prevMousePos{};
        float m_deltaX = 0.0f;
        float m_deltaY = 0.0f;
        bool m_initialized = false;
    };
}
