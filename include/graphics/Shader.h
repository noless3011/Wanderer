#pragma once
#include "pch.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

class Shader {
public:
    Shader(std::filesystem::path path, std::string name, const std::string& entryPoint = "main") : m_path(path), m_name(name), m_entryPoint(entryPoint) {
        m_isCompiled = false;
        m_shaderID = 0;
        m_infoLog[0] = '\0';
        m_sourceCode = loadShaderSource(path);
    }
    ~Shader() {
        if (m_shaderID) {
            glDeleteShader(m_shaderID);
            LOG_INFO("Shader {} (ID: {}) deleted successfully.", m_name, m_shaderID);
        }
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept
        : m_shaderID(other.m_shaderID), m_name(std::move(other.m_name)), m_isCompiled(other.m_isCompiled),
        m_sourceCode(std::move(other.m_sourceCode)), m_path(std::move(other.m_path)), m_entryPoint(std::move(other.m_entryPoint)) {
        std::copy(std::begin(other.m_infoLog), std::end(other.m_infoLog), std::begin(m_infoLog));
        other.m_shaderID = 0; // Reset the moved-from object's shader ID
    }
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            m_shaderID = other.m_shaderID;
            m_name = std::move(other.m_name);
            m_isCompiled = other.m_isCompiled;
            m_sourceCode = std::move(other.m_sourceCode);
            m_path = std::move(other.m_path);
            m_entryPoint = std::move(other.m_entryPoint);
            std::copy(std::begin(other.m_infoLog), std::end(other.m_infoLog), std::begin(m_infoLog));
            other.m_shaderID = 0; // Reset the moved-from object's shader ID
        }
        return *this;
    }

    GLuint getID() const { return m_shaderID; }
    std::string getName() const { return m_name; }
    bool isCompiled() const { return m_isCompiled; }
    const std::string& getInfoLog() const { return m_infoLog; }
    const std::filesystem::path& getPath() const { return m_path; }
    const std::string& getEntryPoint() const { return m_entryPoint; }
    void setEntryPoint(const std::string& entryPoint) { m_entryPoint = entryPoint; }
    void setPath(const std::filesystem::path& path) { m_path = path; }
    const std::string& getSourceCode() const { return m_sourceCode; }
protected:
    virtual void compile() {}

    std::string loadShaderSource(const std::filesystem::path& filePath);

    GLuint m_shaderID;
    std::string m_name;
    bool m_isCompiled;
    std::string m_sourceCode;
    char m_infoLog[512];
    std::filesystem::path m_path;
    std::string m_entryPoint;
};


class VertexShader : public Shader {
public:
    VertexShader(std::filesystem::path path, std::string name, const std::string& entryPoint = "main") : Shader(path, name, entryPoint) {
        compile();
    }
    ~VertexShader() = default;

    VertexShader(const VertexShader&) = delete;
    VertexShader& operator=(const VertexShader&) = delete;
    VertexShader(VertexShader&& other) noexcept;
    VertexShader& operator=(VertexShader&& other) noexcept;

protected:
    void compile() override;
};

class FragmentShader : public Shader {
public:
    FragmentShader(std::filesystem::path path, std::string name, const std::string& entryPoint = "main") : Shader(path, name, entryPoint) {
        compile();
    }

    FragmentShader(const FragmentShader&) = delete;
    FragmentShader& operator=(const FragmentShader&) = delete;
    FragmentShader(FragmentShader&& other) noexcept;
    FragmentShader& operator=(FragmentShader&& other) noexcept;
protected:
    void compile() override;
};


