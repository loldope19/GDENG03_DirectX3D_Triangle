#pragma once
#include <chrono>

namespace dx3d
{
    class EngineTime final
    {
    public:
        static void initialize();
        static void release();

        static void logFrameStart();
        static void logFrameEnd();

        static double getDeltaTime();
        static double getUnscaledDeltaTime();

        static void setTimeScale(float scale);
        static float getTimeScale();

    private:
        EngineTime();
        ~EngineTime();

        EngineTime(const EngineTime&) = delete;
        EngineTime& operator=(const EngineTime&) = delete;
        EngineTime(EngineTime&&) = delete;
        EngineTime& operator=(EngineTime&&) = delete;

        static EngineTime* s_sharedInstance;

        std::chrono::high_resolution_clock::time_point m_frameStartTime;
        std::chrono::high_resolution_clock::time_point m_frameEndTime;

        double m_unscaledDeltaTime; // Delta time without time scale applied
        double m_deltaTime;         // Delta time with time scale applied
        float m_timeScale;          // Current time scale factor
    };
}