#include "pch.h" 
#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include "core/Window.h"
#include "game/Game.h"
#include "spdlog/spdlog.h"
#include <glm.hpp>

// ECS includes
#include "ecs/World.h"



int main() {
	LOG_INFO("Starting the game...");	

	// Initialize the window
	Window window(800, 600, "Wanderer");
	Game game{};

	game.init(window.getGLFWWindow());
	game.run();
	game.cleanup();
    return 0;
}