#include <DX3D/Graphics/Triangle.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Shader.h>

namespace dx3d
{
    struct Vertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    Triangle::Triangle(const GraphicsResourceDesc& gDesc, const Vec4& color, float x, float y) 
        : Mesh(gDesc)
    {
        std::vector<Vertex> vertices = {
            { x + 0.0f,  y + 0.5f, 0.0f, color.x, color.y, color.z, color.w },  // Top vertex
            { x + 0.5f,  y - 0.5f, 0.0f, color.x, color.y, color.z, color.w },  // Bottom right vertex
            { x - 0.5f,  y - 0.5f, 0.0f, color.x, color.y, color.z, color.w }   // Bottom left vertex
        };

        initializeBuffers(vertices);
    }

    void Triangle::render(ID3D11DeviceContext& context)
    {
        context.IASetInputLayout(m_inputLayout.Get());

        ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
        context.IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);

        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context.VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context.PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        context.Draw(3, 0);
    }
}