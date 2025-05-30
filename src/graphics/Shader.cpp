#include "graphics/Shader.h"



std::string Shader::loadShaderSource(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Shader::loadShaderSource: Failed to open shader file: {}", filePath.string());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    if (buffer.fail()) {
        LOG_ERROR("Shader::loadShaderSource: Failed to read shader file: {}", filePath.string());
        return "";
    }
    LOG_INFO("Shader::loadShaderSource: Source code:\n{}", buffer.str());
    return buffer.str();
}
void VertexShader::compile() {
    if (m_isCompiled) {
        LOG_WARN("VertexShader::compile: Shader already compiled. Skipping re-compilation.");
        return;
    }
    LOG_INFO("VertexShader::compile: Compiling vertex shader from path: {}", m_path.string());

    GLenum preError = glGetError();
    if (preError != GL_NO_ERROR) {
        LOG_WARN("VertexShader::compile: Pre-existing OpenGL error before shader compilation: {:#x}", preError);
    }
    unsigned int shaderID = glCreateShader(GL_VERTEX_SHADER);
    if (shaderID == 0) {
        LOG_ERROR("VertexShader::compile: Failed to create vertex shader object (glCreateShader returned 0).");
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOG_ERROR("VertexShader::compile: OpenGL error after glCreateShader failed: {:#x}", err);
        }
        return;
    }
    LOG_INFO("VertexShader::compile: Vertex shader object created successfully (ID: {}).", shaderID);
    const char* sourceCodePtr = m_sourceCode.c_str();
    glShaderSource(shaderID, 1, &sourceCodePtr, nullptr);
    glCompileShader(shaderID);
    LOG_INFO("VertexShader::compile: Vertex shader (ID: {}) compilation attempt finished.", shaderID);
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    LOG_INFO("VertexShader::compile: Vertex shader (ID: {}) GL_COMPILE_STATUS: {}", shaderID, success);
    if (!success) {
        glGetShaderInfoLog(shaderID, sizeof(m_infoLog), nullptr, m_infoLog);
        LOG_ERROR("VertexShader::compile: ERROR::SHADER::VERTEX::COMPILATION_FAILED (ID: {})\n{}", shaderID, m_infoLog);
        glDeleteShader(shaderID);
        m_shaderID = 0;
        return;
    }
    m_shaderID = shaderID;
    m_isCompiled = true;
    LOG_INFO("VertexShader::compile: Vertex shader (ID: {}) compiled successfully.", m_shaderID);
    m_infoLog[0] = '\0';
}

VertexShader::VertexShader(VertexShader&& other) noexcept
    : Shader(std::move(other)) {
}

VertexShader& VertexShader::operator=(VertexShader&& other) noexcept {
    if (this != &other) {
        Shader::operator=(std::move(other));
    }
    return *this;
}

void FragmentShader::compile() {
    if (m_isCompiled) {
        LOG_WARN("FragmentShader::compile: Shader already compiled. Skipping re-compilation.");
        return;
    }
    LOG_INFO("FragmentShader::compile: Compiling fragment shader from path: {}", m_path.string());

    GLenum preError = glGetError();
    if (preError != GL_NO_ERROR) {
        LOG_WARN("FragmentShader::compile: Pre-existing OpenGL error before shader compilation: {:#x}", preError);
    }
    unsigned int shaderID = glCreateShader(GL_FRAGMENT_SHADER);
    if (shaderID == 0) {
        LOG_ERROR("FragmentShader::compile: Failed to create fragment shader object (glCreateShader returned 0).");
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOG_ERROR("FragmentShader::compile: OpenGL error after glCreateShader failed: {:#x}", err);
        }
        return;
    }
    LOG_INFO("FragmentShader::compile: Fragment shader object created successfully (ID: {}).", shaderID);
    const char* sourceCodePtr = m_sourceCode.c_str();
    glShaderSource(shaderID, 1, &sourceCodePtr, nullptr);
    glCompileShader(shaderID);
    LOG_INFO("FragmentShader::compile: Fragment shader (ID: {}) compilation attempt finished.", shaderID);
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    LOG_INFO("FragmentShader::compile: Fragment shader (ID: {}) GL_COMPILE_STATUS: {}", shaderID, success);
    if (!success) {
        glGetShaderInfoLog(shaderID, sizeof(m_infoLog), nullptr, m_infoLog);
        LOG_ERROR("FragmentShader::compile: ERROR::SHADER::FRAGMENT::COMPILATION_FAILED (ID: {})\n{}", shaderID, m_infoLog);
        glDeleteShader(shaderID);
        m_shaderID = 0;
        return;
    }
    m_shaderID = shaderID;
    m_isCompiled = true;
    LOG_INFO("FragmentShader::compile: Fragment shader (ID: {}) compiled successfully.", m_shaderID);
    m_infoLog[0] = '\0';

}

FragmentShader::FragmentShader(FragmentShader&& other) noexcept
    : Shader(std::move(other)) {
}

FragmentShader& FragmentShader::operator=(FragmentShader&& other) noexcept {
    if (this != &other) {
        Shader::operator=(std::move(other));
    }
    return *this;
}
