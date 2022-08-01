#include "shader.h"


ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType) {
    auto shader = ShaderUPtr(new Shader());
    if (!shader->LoadFile(filename, shaderType))
        return nullptr;
    return std::move(shader);
}

Shader::~Shader() {
    if (m_shader) {
        glDeleteShader(m_shader);
    }
}

bool Shader::LoadFile(const std::string& filename, GLenum shaderType) {
    auto result = LoadTextFile(filename);
    if (!result.has_value())
        return false;

    auto& code = result.value();
#ifdef __EMSCRIPTEN__
    // #version 330 core -> #version 300 es\nprecision mediump float;
    std::string version = "#version 330 core";
    int start = code.find(version);
    code.replace(start, version.size(), "#version 300 es\nprecision mediump float;");
#endif
    const char* codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();
    //SPDLOG_INFO("Shader Source\n{}", codePtr);

    // create and compile shader
    m_shader = glCreateShader(shaderType);
    glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
    glCompileShader(m_shader);

    // check compile error
    int success = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}