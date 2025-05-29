#include "pch.h" 
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/Window.h"
#include "game/Game.h"
#include "spdlog/spdlog.h"


int main() {
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