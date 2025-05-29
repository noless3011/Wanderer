#pragma once
#include "include/pch.h"
#include <glad/glad.h>
#include "Shader.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&& other) noexcept;
    Pipeline& operator=(Pipeline&& other) noexcept;

    void attachShader(const Shader& shader);
    void link(bool detachShadersAfterLink = true);
    void detachShader(const Shader& shader);
    void detachAllShaders();

    void use() const;
    static void disuse() { glUseProgram(0); }

    // Getters
    GLuint getID() const { return m_programID; }
    bool isLinked() const { return m_isLinked; }
    const std::string& getInfoLog() const { return m_infoLog; }

    // Uniform setters
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, bool value) const; // Often implemented as int uniform
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

private:
    GLint getUniformLocation(const std::string& name) const;

    GLuint m_programID;
    bool m_isLinked;
    std::string m_infoLog;
    std::vector<GLuint> m_attachedShaderIDs;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};