#pragma once
#include <DX3D/Graphics/GraphicsResource.h>
#include <string>
#include <vector>

namespace dx3d
{
    class Shader
    {
    public:
        enum class Type
        {
            Vertex,
            Pixel
        };

        struct ShaderDesc
        {
            GraphicsResourceDesc graphicsDesc;
            Type type;
            std::string entryPoint;
            std::string target;
        };

        explicit Shader(const ShaderDesc& desc);
        ~Shader();

        bool compile(const std::string& source);
        bool loadFromFile(const std::string& filename);
        
        ID3D11VertexShader* getVertexShader() const { return m_vertexShader.Get(); }
        ID3D11PixelShader* getPixelShader() const { return m_pixelShader.Get(); }
        const std::vector<BYTE>& getByteCode() const { return m_byteCode; }

    private:
        ShaderDesc m_desc;
        std::vector<BYTE> m_byteCode;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    };
} 