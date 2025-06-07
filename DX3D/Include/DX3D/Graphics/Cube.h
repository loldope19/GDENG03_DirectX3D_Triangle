#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Math/Matrix4x4.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct CB_Cube_Data
    {
        Matrix4x4 worldMatrix;
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
    };

    struct CubeVertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    class Cube : public GraphicsResource
    {
    public:
        Cube(const GraphicsResourceDesc& gDesc, const Vec3& rotationAxis, float rotationSpeed);

        void setPosition(const Vec3& pos);
        void setRotation(const Vec3& rot);
        void setScale(const Vec3& scale);

        void update(float dt);
        void render(ID3D11DeviceContext& context, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);

    private:
        void updateWorldMatrix();


    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer; // For MVP matrices
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;

        Matrix4x4 m_worldMatrix;
        Vec3 m_position;
        // Vec3 m_rotation;
        Vec3 m_scale;

        Vec3 m_rotationAxis;
        float m_rotationSpeed;
        float m_accumulatedAngle;
        
        UINT m_stride;
        UINT m_offset;
    };
} 