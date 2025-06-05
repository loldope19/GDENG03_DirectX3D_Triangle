#include <DX3D/Core/EngineTime.h>

namespace dx3d
{
    EngineTime* EngineTime::s_sharedInstance = nullptr;

    void EngineTime::initialize()
    {
        if (s_sharedInstance == nullptr)
        {
            s_sharedInstance = new EngineTime();
        }
    }

    void EngineTime::release()
    {
        delete s_sharedInstance;
        s_sharedInstance = nullptr;
    }

    EngineTime::EngineTime() :
        m_unscaledDeltaTime(0.0),
        m_deltaTime(0.0),
        m_timeScale(1.0f)
    {
        m_frameStartTime = std::chrono::high_resolution_clock::now();
        m_frameEndTime = std::chrono::high_resolution_clock::now();
    }

    EngineTime::~EngineTime()
    {
    }

    void EngineTime::logFrameStart()
    {
        if (s_sharedInstance)
        {
            s_sharedInstance->m_frameStartTime = std::chrono::high_resolution_clock::now();
        }
    }

    void EngineTime::logFrameEnd()
    {
        if (s_sharedInstance)
        {
            s_sharedInstance->m_frameEndTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = s_sharedInstance->m_frameEndTime - s_sharedInstance->m_frameStartTime;
            s_sharedInstance->m_unscaledDeltaTime = elapsed.count();
            s_sharedInstance->m_deltaTime = s_sharedInstance->m_unscaledDeltaTime * s_sharedInstance->m_timeScale;
        }
    }

    double EngineTime::getDeltaTime()
    {
        return s_sharedInstance ? s_sharedInstance->m_deltaTime : 0.0;
    }

    double EngineTime::getUnscaledDeltaTime()
    {
        return s_sharedInstance ? s_sharedInstance->m_unscaledDeltaTime : 0.0;
    }

    void EngineTime::setTimeScale(float scale)
    {
        if (s_sharedInstance)
        {
            if (scale >= 0.0f) {
                s_sharedInstance->m_timeScale = scale;
            }
            else {
                s_sharedInstance->m_timeScale = 0.0f;
            }
        }
    }

    float EngineTime::getTimeScale()
    {
        return s_sharedInstance ? s_sharedInstance->m_timeScale : 1.0f;
    }
}