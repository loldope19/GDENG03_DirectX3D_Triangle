#pragma once
#include <DX3D/Math/Vec3.h>
#include <cmath>
#include <memory>

namespace dx3d
{
    class Matrix4x4
    {
    public:
        Matrix4x4()
        {
            setIdentity();
        }

        void setIdentity()
        {
            ::memset(m_mat, 0, sizeof(float) * 16);
            m_mat[0][0] = 1.0f;
            m_mat[1][1] = 1.0f;
            m_mat[2][2] = 1.0f;
            m_mat[3][3] = 1.0f;
        }

        void setTranslation(const Vec3& translation)
        {
            setIdentity();
            m_mat[3][0] = translation.x;
            m_mat[3][1] = translation.y;
            m_mat[3][2] = translation.z;
        }

        void setScale(const Vec3& scale)
        {
            setIdentity();
            m_mat[0][0] = scale.x;
            m_mat[1][1] = scale.y;
            m_mat[2][2] = scale.z;
        }

        void setRotationX(float x)
        {
            setIdentity();
            m_mat[1][1] = cos(x);
            m_mat[1][2] = sin(x);
            m_mat[2][1] = -sin(x);
            m_mat[2][2] = cos(x);
        }

        void setRotationY(float y)
        {
            setIdentity();
            m_mat[0][0] = cos(y);
            m_mat[0][2] = -sin(y);
            m_mat[2][0] = sin(y);
            m_mat[2][2] = cos(y);
        }

        void setRotationZ(float z)
        {
            setIdentity();
            m_mat[0][0] = cos(z);
            m_mat[0][1] = sin(z);
            m_mat[1][0] = -sin(z);
            m_mat[1][1] = cos(z);
        }

        void setRotation(const Vec3& axis, float angle)
        {
            Vec3 n_axis = axis;
            n_axis.normalize();

            const float c = cos(angle);
            const float s = sin(angle);
            const float omc = 1.0f - c;

            const float x = n_axis.x;
            const float y = n_axis.y;
            const float z = n_axis.z;

            m_mat[0][0] = c + x * x * omc;
            m_mat[0][1] = x * y * omc - z * s;
            m_mat[0][2] = x * z * omc + y * s;
            m_mat[0][3] = 0.0f;

            m_mat[1][0] = y * x * omc + z * s;
            m_mat[1][1] = c + y * y * omc;
            m_mat[1][2] = y * z * omc - x * s;
            m_mat[1][3] = 0.0f;

            m_mat[2][0] = z * x * omc - y * s;
            m_mat[2][1] = z * y * omc + x * s;
            m_mat[2][2] = c + z * z * omc;
            m_mat[2][3] = 0.0f;

            m_mat[3][0] = 0.0f;
            m_mat[3][1] = 0.0f;
            m_mat[3][2] = 0.0f;
            m_mat[3][3] = 1.0f;
        }

        void transpose()
        {
            Matrix4x4 temp = *this;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m_mat[i][j] = temp.m_mat[j][i];
        }

        static Matrix4x4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
        {
            Vec3 zaxis(target.x - eye.x, target.y - eye.y, target.z - eye.z);
            zaxis.normalize();

            Vec3 xaxis(up.y * zaxis.z - up.z * zaxis.y, up.z * zaxis.x - up.x * zaxis.z, up.x * zaxis.y - up.y * zaxis.x); // Cross product
            xaxis.normalize();

            Vec3 yaxis(zaxis.y * xaxis.z - zaxis.z * xaxis.y, zaxis.z * xaxis.x - zaxis.x * xaxis.z, zaxis.x * xaxis.y - zaxis.y * xaxis.x); // Cross product

            Matrix4x4 viewMatrix;
            viewMatrix.setIdentity();

            viewMatrix.m_mat[0][0] = xaxis.x; viewMatrix.m_mat[0][1] = xaxis.y; viewMatrix.m_mat[0][2] = xaxis.z;
            viewMatrix.m_mat[1][0] = yaxis.x; viewMatrix.m_mat[1][1] = yaxis.y; viewMatrix.m_mat[1][2] = yaxis.z;
            viewMatrix.m_mat[2][0] = zaxis.x; viewMatrix.m_mat[2][1] = zaxis.y; viewMatrix.m_mat[2][2] = zaxis.z;

            viewMatrix.m_mat[3][0] = -(xaxis.x * eye.x + xaxis.y * eye.y + xaxis.z * eye.z);
            viewMatrix.m_mat[3][1] = -(yaxis.x * eye.x + yaxis.y * eye.y + yaxis.z * eye.z);
            viewMatrix.m_mat[3][2] = -(zaxis.x * eye.x + zaxis.y * eye.y + zaxis.z * eye.z);

            return viewMatrix;
        }

        static Matrix4x4 perspective(float fov, float aspect, float zNear, float zFar)
        {
            Matrix4x4 projMatrix;
            float fovRad = fov * 3.1415926535f / 180.0f;
            float tanHalfFov = tan(fovRad / 2.0f);

            projMatrix.m_mat[0][0] = 1.0f / (aspect * tanHalfFov);
            projMatrix.m_mat[1][1] = 1.0f / tanHalfFov;
            projMatrix.m_mat[2][2] = zFar / (zFar - zNear);
            projMatrix.m_mat[2][3] = 1.0f;
            projMatrix.m_mat[3][2] = (-zNear * zFar) / (zFar - zNear);
            projMatrix.m_mat[3][3] = 0.0f;

            return projMatrix;
        }

        Matrix4x4 operator*(const Matrix4x4& other)
        {
            Matrix4x4 result;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    result.m_mat[i][j] = m_mat[i][0] * other.m_mat[0][j] + m_mat[i][1] * other.m_mat[1][j] +
                        m_mat[i][2] * other.m_mat[2][j] + m_mat[i][3] * other.m_mat[3][j];
                }
            }
            return result;
        }

    public:
        float m_mat[4][4];
    };
}