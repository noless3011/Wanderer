#pragma once
#include "pch.h"
#include <glad/glad.h>
#include "graphics/Shader.h"
#include "spdlog/spdlog.h"


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
    // These methods typically require the pipeline to be in use.
    // Ensure glm headers (e.g., <glm/glm.hpp>, <glm/gtc/type_ptr.hpp>) are included
    // if not already handled by pch.h, for glm::vec and glm::mat types.
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, bool value) const; // Often implemented as int uniform
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;
    // Add more overloads as needed (e.g., arrays, different vector/matrix types)

private:
    // Helper to get uniform location, potentially caching it.
    GLint getUniformLocation(const std::string& name) const;

    GLuint m_programID;
    bool m_isLinked;
    std::string m_infoLog; // Stores linking errors or warnings.
    // Store shader IDs for potential detachment after linking.
    // Requires <vector>
    std::vector<GLuint> m_attachedShaderIDs;
    // Cache for uniform locations to avoid repeated lookups.
    // Requires <string>, <unordered_map>. Marked mutable to allow modification in const methods.
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};