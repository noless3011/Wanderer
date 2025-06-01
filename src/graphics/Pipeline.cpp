#include "graphics/Pipeline.h"


Pipeline::Pipeline() : m_programID(0), m_isLinked(false) {
    m_programID = glCreateProgram();
    if (m_programID == 0) {
        LOG_ERROR("Pipeline::Pipeline: Failed to create shader program (glCreateProgram returned 0).");
    }
    else {
        LOG_INFO("Pipeline::Pipeline: Shader program created successfully with ID: {}", m_programID);
    }
}

Pipeline::~Pipeline() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
        LOG_INFO("Pipeline::~Pipeline: Shader program (ID: {}) deleted successfully.", m_programID);
    }
    else {
        LOG_WARN("Pipeline::~Pipeline: No shader program to delete.");
    }
}

void Pipeline::attachShader(std::unique_ptr<Shader> shader) {
    if (m_programID == 0) {
        LOG_ERROR("Pipeline::attachShader: Cannot attach shader to a non-existent program (m_programID is 0).");
        return;
    }
    if (!shader) {
        LOG_ERROR("Pipeline::attachShader: Cannot attach a null shader.");
        return;
    }
    GLuint shaderID = shader->getID();
    if (shaderID == 0) {
        LOG_ERROR("Pipeline::attachShader: Cannot attach shader '{}' with ID 0. It might not have been compiled successfully.", shader->getName());
        return;
    }

    // Check if shader is already attached
    for (const auto& attachedShader : m_attachedShaders) {
        if (attachedShader->getID() == shaderID) {
            LOG_WARN("Pipeline::attachShader: Shader '{}' (ID: {}) is already attached to program (ID: {}).", shader->getName(), shaderID, m_programID);
            return;
        }
    }

    glAttachShader(m_programID, shaderID);

    // Verify attachment (optional, but good for debugging)
    GLint success;
    glGetProgramiv(m_programID, GL_ATTACHED_SHADERS, &success);

    m_attachedShaders.push_back(std::move(shader)); // shader is now invalid in the caller
    LOG_INFO("Pipeline::attachShader: Shader (ID: {}) attached successfully to program (ID: {}).", shaderID, m_programID);
}

void Pipeline::link(bool detachShaderAfterLink) {
    glLinkProgram(m_programID);

    int successLink;
    char infoLogLink[512];
    glGetProgramiv(m_programID, GL_LINK_STATUS, &successLink);
    if (!successLink) {
        glGetProgramInfoLog(m_programID, 512, NULL, infoLogLink);
        LOG_ERROR("Pipeline::link: ERROR::SHADER::PROGRAM::LINKING_FAILED(ID : {})\n{}", m_programID, infoLogLink);
        glDeleteProgram(m_programID);
        m_programID = 0;
        m_isLinked = false;
    }
    else {
        m_isLinked = true;
        LOG_INFO("Pipeline::link: Successfully linked shader program (ID: {}).", m_programID);
        if (detachShaderAfterLink) {
            detachAllShaders();
            LOG_INFO("Pipeline::link: All shaders detached after linking program (ID: {}).", m_programID);
        }
    }

    if (m_programID != 0 && m_isLinked) {
        LOG_INFO("Pipeline::link: Successfully loaded and configured pipeline (ID: {}).", m_programID);
    }
    else {
        LOG_ERROR("Pipeline::link: Failed to configure shader pipeline (ID: {}).", m_programID);
    }
}

GLuint Pipeline::findShaderID(std::string name) {
    if (m_attachedShaders.empty()) {
        LOG_INFO("No shader in the pipeline");
        return 0;
    }
    for (int i = 0; i < m_attachedShaders.size(); i++) {
        if (m_attachedShaders[i]->getName() == name) {
            return m_attachedShaders[i]->getID();
        }
    }
    LOG_WARN("No shader with the name {} found", name);
    return 0;
}

void Pipeline::detachAllShaders() {
    if (m_attachedShaders.empty()) {
        LOG_INFO("No shader to detach");
        return;
    }
    if (m_programID == 0) {
        LOG_ERROR("Pipeline hasnt been created");
        return;
    }
    for (int i = 0; i < m_attachedShaders.size(); i++) {
        glDetachShader(m_programID, m_attachedShaders[i]->getID());
    }
}

void Pipeline::use() const
{
    if (m_programID == 0) {
        LOG_ERROR("Pipeline::use: Shader program has not been created or linked.");
        return;
    }
    glUseProgram(m_programID);
    LOG_INFO("Pipeline::use: Using shader program (ID: {}).", m_programID);
}


void Pipeline::detachShader(std::unique_ptr<Shader> shader) {
    if (!shader->getID()) {
        LOG_ERROR("Shader {} hasn't loaded", shader->getName());
    }
    glDetachShader(m_programID, shader->getID());
    LOG_INFO("Sucessfully detach shader {}", shader->getName());
}

void Pipeline::setUniform(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Pipeline::setUniform(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Pipeline::setUniform(const std::string& name, bool value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value ? 1 : 0);
    }
}

void Pipeline::setUniform(const std::string& name, const glm::vec2& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, &value[0]);
    }
}

void Pipeline::setUniform(const std::string& name, const glm::vec3& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
}

void Pipeline::setUniform(const std::string& name, const glm::vec4& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, &value[0]);
    }
}

void Pipeline::setUniform(const std::string& name, const glm::mat3& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

void Pipeline::setUniform(const std::string& name, const glm::mat4& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

GLint Pipeline::getUniformLocation(const std::string& name) const {
    auto it = m_uniformLocationCache.find(name);
    if (it != m_uniformLocationCache.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location == -1) {
        LOG_WARN("Pipeline::getUniformLocation: Uniform '{}' not found in shader program (ID: {}).", name, m_programID);
    }

    m_uniformLocationCache[name] = location;
    return location;
}



