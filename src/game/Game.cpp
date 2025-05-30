#include "pch.h"
#include "game/Game.h"
#include <iostream>
#include <glad/glad.h>
#include "spdlog/spdlog.h"
#include "graphics/Pipeline.h"
#include "graphics/Shader.h"

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
    Pipeline tempPipeline;

    try {
        LOG_INFO("Current working directory: {}", std::filesystem::current_path().string());
        std::filesystem::path fragmentShaderPath = "D:\\ProgrammingProjects\\c++\\Wanderer\\assets\\shader\\fragment\\fragment.frag";
        std::filesystem::path vertexShaderPath = "D:\\ProgrammingProjects\\c++\\Wanderer\\assets\\shader\\vertex\\vertex.vert";
        // Create shader objects. Compilation happens in their constructors.
        std::unique_ptr<Shader> vertexShader = std::make_unique<VertexShader>(vertexShaderPath, "VertexShader");
        std::unique_ptr<Shader> fragmentShader = std::make_unique<FragmentShader>(fragmentShaderPath, "FragmentShader");

        // Check if shaders compiled successfully
        if (!vertexShader->isCompiled()) {
            spdlog::error("Vertex shader ({}) compilation failed: {}", vertexShaderPath.string(), vertexShader->getInfoLog());
            m_shaderProgram = 0; // Ensure m_shaderProgram is 0 on failure
            return;
        }
        if (!fragmentShader->isCompiled()) {
            spdlog::error("Fragment shader ({}) compilation failed: {}", fragmentShaderPath.string(), fragmentShader->getInfoLog());
            m_shaderProgram = 0; // Ensure m_shaderProgram is 0 on failure
            return;
        }

        LOG_INFO("Start attaching shaders to pipeline.");
        tempPipeline.attachShader(std::move(vertexShader));
        tempPipeline.attachShader(std::move(fragmentShader));
        LOG_INFO("Shaders attached to pipeline. Vertex Shader ID: {}, Fragment Shader ID: {}",
            tempPipeline.findShaderID("VertexShader"), tempPipeline.findShaderID("FragmentShader"));
        tempPipeline.link();

        if (tempPipeline.isLinked()) {
            m_shaderProgram = tempPipeline.getID(); // Get the program ID
            spdlog::info("Shader program linked successfully. ID: {}", m_shaderProgram);
        }
        else {
            spdlog::error("Shader pipeline linking failed: {}", tempPipeline.getInfoLog());
            m_shaderProgram = 0;
        }
        tempPipeline.use();
    }
    catch (const std::exception& e) {
        spdlog::error("Exception during shader loading: {}", e.what());
        m_shaderProgram = 0;
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


