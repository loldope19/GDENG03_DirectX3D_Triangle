#pragma once
#include <DirectXMath.h>

namespace dx3d
{
    class Camera
    {
    public:
        Camera();

        void setPerspective(float fovY, float aspect, float nearZ, float farZ);
        void setPosition(const DirectX::XMFLOAT3& position);
        void setRotation(float yaw, float pitch);
        void move(const DirectX::XMFLOAT3& offset);
        void rotate(float yawOffset, float pitchOffset);

        DirectX::XMMATRIX getViewMatrix();
        DirectX::XMMATRIX getProjectionMatrix() const;
        DirectX::XMFLOAT3 getPosition() const;

    private:
        void updateView();

        DirectX::XMFLOAT3 m_position;
        float m_yaw;   // left/right rotation
        float m_pitch; // up/down rotation

        float m_fovY;
        float m_aspectRatio;
        float m_nearZ;
        float m_farZ;

        mutable DirectX::XMMATRIX m_cachedViewMatrix;
        mutable bool m_viewDirty;
    };
}
