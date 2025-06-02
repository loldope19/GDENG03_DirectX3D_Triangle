#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct RectangleVertex
    {
        float x, y, z;    // position
        float r, g, b, a; // color
    };

    class Rectangle : public GraphicsResource
    {
    public:
        Rectangle(const GraphicsResourceDesc& gDesc);

        bool initializeSharedResources();                                       // sets up all shaders (only once)
        void createRectangle(const std::vector<RectangleVertex>& vertices);     // creates rectangle and subjects it to buffer hell
        void render(ID3D11DeviceContext& context);                              // renders all rectangles
        size_t getRectangleCount() const { return m_vertexBuffers.size(); }     // gets how many rectangles there are

    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_indexBuffers;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        UINT m_stride;
        UINT m_offset;

        bool m_sharedResourcesInitialized = false;
    };
}