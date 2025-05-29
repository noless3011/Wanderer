#include "pch.h" 
#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include "core/Window.h"
#include "game/Game.h"
#include "spdlog/spdlog.h"
#include <glm.hpp>


int main() {

    // test glm 
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::vec3 direction(0.0f, 1.0f, 0.0f);
    glm::vec3 newPosition = position + direction * 5.0f; // Move position in the direction by 5 units
    spdlog::info("New Position: ({}, {}, {})", newPosition.x, newPosition.y, newPosition.z);
    Window window(800, 600, "Wanderer");
    window.setResizable(true);

    if (!window.getGLFWWindow()) {
        spdlog::error("Failed to create GLFW window");
        return -1;
    }
    Game game;

    game.init(window.getGLFWWindow());
    game.run();
    game.cleanup();
    return 0;
}