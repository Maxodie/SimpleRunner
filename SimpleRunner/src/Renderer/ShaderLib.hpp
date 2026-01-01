#pragma once

#include "Core/Core.hpp"
#include "Utils/FileSystem.hpp"
#include "nvrhi/nvrhi.h"
#include <shaderc/shaderc.hpp>

namespace SR
{

//matches nvrhi::ShaderType
enum class ShaderType : uint16_t
{
    NONE = 0x0000,
    COMPUTE = 0x0020,

    VERTEX  = 0x0001,
    HULL = 0x0002,
    DOMAIN_ = 0x0004,
    GEOMETRY = 0x0008,
    PIXEL = 0x0010,
    AMPLIFICATION = 0x0040,
    MESH = 0x0080,
    ALLGRAPHICS = 0x00DF,

    RAYGENERATION = 0x0100,
    ANYHIT = 0x0200,
    CLOSESTHIT = 0x0400,
    MISS = 0x0800,
    INTERSECTION = 0x1000,
    CALLABLE = 0x2000,
    ALLRAYTRACING = 0x3F00,

    ALL = 0x3FFF,
};


class ShaderLib
{

public:
    class Shader
    {
    public:
        Shader() = delete;
        Shader(shaderc::SpvCompilationResult& compiledShader, ShaderType type);
        ~Shader();

        SR_INLINE ShaderType GetType() const
        {
            return m_type;
        }

        SR_INLINE const nvrhi::ShaderHandle& GetHandle() const
        {
            return m_handle;
        }

    private:
        nvrhi::ShaderHandle m_handle;
        ShaderType m_type;
    };

    bool AttachShader(Path& path, ShaderType type);
    void DestroyShaders();

    SR_INLINE const std::vector<Shader> GetShaders() const
    {
        return m_shaders;
    }

private:
    shaderc::SpvCompilationResult CompileShader(const std::string& name, const std::string& shaderToCompile, ShaderType type);

private:
    std::vector<Shader> m_shaders;
    shaderc::Compiler m_compiler;
};

}
