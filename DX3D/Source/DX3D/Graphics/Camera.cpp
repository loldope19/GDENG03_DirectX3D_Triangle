#include <DX3D/Graphics/Camera.h>
#include <cmath>
#include <algorithm>

using namespace DirectX;

namespace dx3d
{
    Camera::Camera()
        : m_position(0.0f, 0.0f, -5.0f),
        m_yaw(0.0f),
        m_pitch(0.0f),
        m_fovY(XM_PIDIV4),
        m_aspectRatio(1.77f),
        m_nearZ(0.1f),
        m_farZ(100.0f),
        m_viewDirty(true)
    {
    }

    void Camera::setPerspective(float fovY, float aspect, float nearZ, float farZ)
    {
        m_fovY = fovY;
        m_aspectRatio = aspect;
        m_nearZ = nearZ;
        m_farZ = farZ;
    }

    void Camera::setPosition(const XMFLOAT3& position)
    {
        m_position = position;
        m_viewDirty = true;
    }

    void Camera::setRotation(float yaw, float pitch)
    {
        m_yaw = yaw;
        m_pitch = pitch;
        m_viewDirty = true;
    }

    void Camera::move(const XMFLOAT3& offset)
    {
        m_position.x += offset.x;
        m_position.y += offset.y;
        m_position.z += offset.z;
        m_viewDirty = true;
    }

    void Camera::rotate(float yawOffset, float pitchOffset)
    {
        m_yaw += yawOffset;
        m_pitch += pitchOffset;

        // Clamp pitch to avoid flipping
        const float pitchLimit = XM_PIDIV2 - 0.01f;
        m_pitch = std::clamp(m_pitch, -pitchLimit, pitchLimit);

        m_viewDirty = true;
    }

    XMMATRIX Camera::getViewMatrix()
    {
        if (m_viewDirty)
            updateView();

        return m_cachedViewMatrix;
    }

    XMMATRIX Camera::getProjectionMatrix() const
    {
        return XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearZ, m_farZ);
    }

    XMFLOAT3 Camera::getPosition() const
    {
        return m_position;
    }

    void Camera::updateView()
    {
        XMVECTOR pos = XMLoadFloat3(&m_position);

        // Calculate look direction from yaw and pitch
        XMVECTOR forward = XMVectorSet(
            cosf(m_pitch) * sinf(m_yaw),
            sinf(m_pitch),
            cosf(m_pitch) * cosf(m_yaw),
            0.0f
        );

        XMVECTOR up = XMVectorSet(0, 1, 0, 0);
        XMVECTOR lookAt = XMVectorAdd(pos, forward);

        m_cachedViewMatrix = XMMatrixLookAtLH(pos, lookAt, up);
        m_viewDirty = false;
    }
}