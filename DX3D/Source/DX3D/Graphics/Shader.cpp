#include <DX3D/Graphics/Shader.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

namespace dx3d
{
    Shader::Shader(const ShaderDesc& desc) : m_desc(desc)
    {
    }

    Shader::~Shader()
    {
    }

    bool Shader::compile(const std::string& source)
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG;
        flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

        HRESULT hr = D3DCompile(
            source.c_str(),
            source.length(),
            nullptr,
            nullptr,
            nullptr,
            m_desc.entryPoint.c_str(),
            m_desc.target.c_str(),
            flags,
            0,
            &shaderBlob,
            &errorBlob
        );

        if (FAILED(hr))
        {
            if (errorBlob)
            {
                //DX3DLogError(static_cast<char*>(errorBlob->GetBufferPointer()));
            }
            return false;
        }

        m_byteCode.resize(shaderBlob->GetBufferSize());
        memcpy(m_byteCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

        if (m_desc.type == Type::Vertex)
        {
            hr = m_desc.graphicsDesc.device.CreateVertexShader(
                m_byteCode.data(),
                m_byteCode.size(),
                nullptr,
                &m_vertexShader
            );
        }
        else
        {
            hr = m_desc.graphicsDesc.device.CreatePixelShader(
                m_byteCode.data(),
                m_byteCode.size(),
                nullptr,
                &m_pixelShader
            );
        }

        if (FAILED(hr))
        {
            char errorMsg[256];
            sprintf_s(errorMsg, "Failed to create %s shader. HRESULT: 0x%08X", 
                m_desc.type == Type::Vertex ? "vertex" : "pixel", hr);
            // DX3DLogError(errorMsg);
            return false;
        }

        return true;
    }

    bool Shader::loadFromFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            // DX3DLogError("Failed to open shader file");
            return false;
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        file.read(buffer.data(), size);

        return compile(std::string(buffer.data(), size));
    }
} 