#pragma once
#include "pch.h"
#include <glad/glad.h>
#include "spdlog/spdlog.h"

class Shader {
public:
    Shader(std::filesystem::path path, const std::string& entryPoint = "main");
    virtual ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    GLuint getID() const { return m_shaderID; }
    bool isCompiled() const { return m_isCompiled; }
    const std::string& getInfoLog() const { return m_infoLog; }
    const std::filesystem::path& getPath() const { return m_path; }
    const std::string& getEntryPoint() const { return m_entryPoint; }
    void setEntryPoint(const std::string& entryPoint) { m_entryPoint = entryPoint; }
    void setPath(const std::filesystem::path& path) { m_path = path; }
    void compile();
protected:
    virtual void compile(const std::string& sourceCode) = 0;

    std::string loadShaderSource(const std::filesystem::path& filePath);

    GLuint m_shaderID;
    bool m_isCompiled;
    std::string m_infoLog;
    std::filesystem::path m_path;
    std::string m_entryPoint;
};


class VertexShader : public Shader {
public:
    VertexShader(std::filesystem::path path, const std::string& entryPoint = "main");
    ~VertexShader() override;

    VertexShader(const VertexShader&) = delete;
    VertexShader& operator=(const VertexShader&) = delete;
    VertexShader(VertexShader&& other) noexcept;
    VertexShader& operator=(VertexShader&& other) noexcept;

protected:
    void compile(const std::string& sourceCode) override;
};

class FragmentShader : public Shader {
public:
    FragmentShader(std::filesystem::path path, const std::string& entryPoint = "main");
    ~FragmentShader() override;

    FragmentShader(const FragmentShader&) = delete;
    FragmentShader& operator=(const FragmentShader&) = delete;
    FragmentShader(FragmentShader&& other) noexcept;
    FragmentShader& operator=(FragmentShader&& other) noexcept;
protected:
    void compile(const std::string& sourceCode) override;
};


