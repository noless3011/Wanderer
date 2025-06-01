#pragma once
#include <glfw3.h>
#include "graphics/Pipeline.h"
#include "graphics/Shader.h"
#include <glm.hpp>

class Game {
public:
    Game();
    ~Game();
    void init(GLFWwindow* window);
    void run();
    void cleanup();
private:
    void processInput();
    void update(float deltaTime);
    void render();

    void loadShaders();
    void setupGameObjects();

    GLFWwindow* m_window;

    unsigned int m_shaderProgram;
    unsigned int m_VAO;
    unsigned int m_VBO;

    float m_lastFrameTime;

    Pipeline m_pipeline;
};
