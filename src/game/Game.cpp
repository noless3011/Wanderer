#include "pch.h"
#include "game/Game.h"
#include <iostream>
#include <glad/glad.h>
#include "spdlog/spdlog.h"


const char* gameVertextShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* gameFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.6f, 1.0f); // Yellow color for the triangle\n"
"}\0";

Game::Game() : m_window(nullptr), m_shaderProgram(0), m_VAO(0), m_VBO(0), m_lastFrameTime(0.0f) {

}

Game::~Game() {
    cleanup();
}

void Game::init(GLFWwindow* window) {
    m_window = window;
    if (!m_window) {
        spdlog::error("ERROR::GAME::INIT: window is null");
        return;
    }
    spdlog::info("Game initialized with window. IMPORTANT: Ensure gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) was called successfully AFTER glfwMakeContextCurrent in your main setup code (e.g., main.cpp or Window class).");

    loadShaders();

    if (m_shaderProgram != 0) {
        spdlog::info("Shaders loaded and shader program ID is: {}. Proceeding to set up game objects.", m_shaderProgram);
        setupGameObjects();
        spdlog::info("Game objects set up successfully.");
        m_lastFrameTime = static_cast<float>(glfwGetTime());
    }
    else {
        spdlog::error("Shader loading failed or was incomplete. Shader program ID is 0. Game objects will not be set up.");
    }
}

void Game::loadShaders() {
    spdlog::info("loadShaders: Starting shader loading process.");

    GLenum preError = glGetError();
    if (preError != GL_NO_ERROR) {
        spdlog::warn("loadShaders: Pre-existing OpenGL error before creating shaders: {:#x}", preError);
    }

    spdlog::info("loadShaders: About to call glCreateShader(GL_VERTEX_SHADER).");
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    spdlog::info("loadShaders: glCreateShader(GL_VERTEX_SHADER) returned ID: {}", vertexShaderId);

    if (vertexShaderId == 0) {
        spdlog::error("loadShaders: Failed to create vertex shader object (glCreateShader returned 0).");
        spdlog::error("This typically means GLAD is not initialized or there's no valid OpenGL context.");
        GLenum err_vs_create = glGetError();
        if (err_vs_create != GL_NO_ERROR) {
            spdlog::error("loadShaders: OpenGL error after vertex glCreateShader failed: {:#x}", err_vs_create);
        }
        m_shaderProgram = 0; // Ensure program is marked as invalid
        return;
    }
    spdlog::info("loadShaders: Vertex shader object created successfully (ID: {}).", vertexShaderId);

    spdlog::info("loadShaders: Compiling vertex shader (ID: {})...", vertexShaderId);
    glShaderSource(vertexShaderId, 1, &gameVertextShaderSource, NULL);
    glCompileShader(vertexShaderId);
    spdlog::info("loadShaders: Vertex shader (ID: {}) compilation attempt finished.", vertexShaderId);

    int success_vs;
    char infoLog_vs[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success_vs);
    spdlog::info("loadShaders: Vertex shader (ID: {}) GL_COMPILE_STATUS: {}", vertexShaderId, success_vs);
    if (!success_vs) {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog_vs);
        spdlog::error("loadShaders: ERROR::SHADER::VERTEX::COMPILATION_FAILED (ID: {})\n{}", vertexShaderId, infoLog_vs);
        glDeleteShader(vertexShaderId);
        m_shaderProgram = 0;
        return;
    }

    spdlog::info("loadShaders: About to call glCreateShader(GL_FRAGMENT_SHADER).");
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    spdlog::info("loadShaders: glCreateShader(GL_FRAGMENT_SHADER) returned ID: {}", fragmentShaderId);

    if (fragmentShaderId == 0) {
        spdlog::error("loadShaders: Failed to create fragment shader object (glCreateShader returned 0).");
        GLenum err_fs_create = glGetError();
        if (err_fs_create != GL_NO_ERROR) {
            spdlog::error("loadShaders: OpenGL error after fragment glCreateShader failed: {:#x}", err_fs_create);
        }
        glDeleteShader(vertexShaderId);
        m_shaderProgram = 0;
        return;
    }
    spdlog::info("loadShaders: Fragment shader object created successfully (ID: {}).", fragmentShaderId);

    spdlog::info("loadShaders: Compiling fragment shader (ID: {})...", fragmentShaderId);
    glShaderSource(fragmentShaderId, 1, &gameFragmentShaderSource, NULL);
    glCompileShader(fragmentShaderId);
    spdlog::info("loadShaders: Fragment shader (ID: {}) compilation attempt finished.", fragmentShaderId);

    int success_fs;
    char infoLog_fs[512];
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success_fs);
    spdlog::info("loadShaders: Fragment shader (ID: {}) GL_COMPILE_STATUS: {}", fragmentShaderId, success_fs);
    if (!success_fs) {
        glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog_fs);
        spdlog::error("loadShaders: ERROR::SHADER::FRAGMENT::COMPILATION_FAILED (ID: {})\n{}", fragmentShaderId, infoLog_fs);
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        m_shaderProgram = 0;
        return;
    }

    spdlog::info("loadShaders: Creating shader program...");
    m_shaderProgram = glCreateProgram(); // Assign to member variable
    if (m_shaderProgram == 0) {
        spdlog::error("loadShaders: Failed to create shader program (glCreateProgram returned 0).");
        GLenum err_prog_create = glGetError();
        if (err_prog_create != GL_NO_ERROR) {
            spdlog::error("loadShaders: OpenGL error after glCreateProgram failed: {:#x}", err_prog_create);
        }
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        return;
    }
    spdlog::info("loadShaders: Shader program created (ID: {}). Attaching shaders...", m_shaderProgram);

    glAttachShader(m_shaderProgram, vertexShaderId);
    glAttachShader(m_shaderProgram, fragmentShaderId);
    spdlog::info("loadShaders: Shaders attached. Linking program (ID: {})...", m_shaderProgram);
    glLinkProgram(m_shaderProgram);
    spdlog::info("loadShaders: Program (ID: {}) linking attempt finished.", m_shaderProgram);

    int success_link;
    char infoLog_link[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success_link);
    spdlog::info("loadShaders: Program (ID: {}) GL_LINK_STATUS: {}", m_shaderProgram, success_link);
    if (!success_link) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog_link);
        spdlog::error("loadShaders: ERROR::SHADER::PROGRAM::LINKING_FAILED (ID: {})\n{}", m_shaderProgram, infoLog_link);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0; // Mark as failed
    }

    spdlog::info("loadShaders: Deleting individual shader objects (Vertex ID: {}, Fragment ID: {})...", vertexShaderId, fragmentShaderId);
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    spdlog::info("loadShaders: Individual shader objects deleted.");

    if (m_shaderProgram != 0 && success_link) { // Check m_shaderProgram as it might have been set to 0 on link failure
        spdlog::info("loadShaders: Shader program (ID: {}) configured successfully.", m_shaderProgram);
    }
    else {
        spdlog::error("loadShaders: Shader program configuration failed.");
        // m_shaderProgram should already be 0 if there was a critical failure
    }
}


void Game::setupGameObjects() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Game::run() {
    while (!glfwWindowShouldClose(m_window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - m_lastFrameTime;
        m_lastFrameTime = currentFrameTime;

        processInput();
        update(deltaTime);
        render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

void Game::update(float deltaTime [[maybe_unused]] ) {

}

void Game::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Game::cleanup() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}


