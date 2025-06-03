#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
    struct CubeVertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    class Cube : public GraphicsResource
    {
    public:
        Cube(const GraphicsResourceDesc& gDesc);

        bool initializeSharedResources();                                       // sets up all shaders (only once)
        void createCube(const std::vector<CubeVertex>& vertices);              // creates cube and subjects it to buffer hell
        void render(ID3D11DeviceContext& context);                              // renders all cubes
        size_t getCubeCount() const { return m_vertexBuffers.size(); }         // gets how many cubes there are

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