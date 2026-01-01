#include "Renderer/ShaderLib.hpp"
#include "Renderer/RendererAPI.hpp"

namespace SR
{

ShaderLib::Shader::Shader(shaderc::SpvCompilationResult& compiledShader, ShaderType type)
{
    m_type = type;
    m_handle = RendererAPI::GetContext().GetHandle()->createShader(
        nvrhi::ShaderDesc().setShaderType(static_cast<nvrhi::ShaderType>(type)),
        compiledShader.cbegin(), (compiledShader.cend() - compiledShader.cbegin()) * sizeof(uint32_t)
    );
}

ShaderLib::Shader::~Shader()
{
    m_handle = nullptr;
}

bool ShaderLib::AttachShader(Path& path, ShaderType type)
{
    std::string data;
    FileSystem::SyncReadAtPathAsString(path, data);

    shaderc::SpvCompilationResult result = CompileShader(path.filename().string(), data, type);
    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        CORE_LOG_ERROR("failed to compile shaders %s code: %d, error %s, content: %s",
                       path.filename().string().c_str(), result.GetCompilationStatus(), result.GetErrorMessage().c_str(), data.c_str());
        return false;
    }

    Shader shader = Shader{result, type};
    m_shaders.push_back(shader);

    return true;
}

void ShaderLib::DestroyShaders()
{
    m_shaders.clear();
}

shaderc::SpvCompilationResult ShaderLib::CompileShader(const std::string& name, const std::string& shaderToCompile, ShaderType type)
{
    shaderc_shader_kind kind = shaderc_glsl_vertex_shader;

    if(type == ShaderType::PIXEL)
    {
        kind = shaderc_glsl_fragment_shader;
    }
    else if(type != ShaderType::VERTEX)
    {
        CORE_LOG_ERROR("given shader type is not handled right now. processing it as a Vertex shader by default");
    }

    return m_compiler.CompileGlslToSpv(
        shaderToCompile.c_str(), shaderToCompile.size(),
        kind,
        name.c_str(),
        shaderc::CompileOptions()
    );
}

}
