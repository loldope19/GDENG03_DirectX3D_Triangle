#pragma once

#include <DX3D/Graphics/Mesh.h>
#include <DX3D/Math/Vec4.h>

namespace dx3d
{
    class Triangle : public Mesh
    {
    public:
        Triangle(const GraphicsResourceDesc& gDesc, 
                const Vec4& color = Vec4(1.0f, 1.0f, 0.5f, 1.0f),
                float x = 0.0f, float y = 0.0f);
    };
} 