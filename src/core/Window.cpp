#include "pch.h"
#include "core/Window.h"
#include "spdlog/spdlog.h"

Window::Window(int width, int height, const std::string& title, bool resizable)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title), m_resizable(resizable) {
    init(resizable);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() {
    if (m_window) {
        glfwSwapBuffers(m_window);
        return;
    }
    else {
        spdlog::error("Window is not initialized. Cannot swap buffers.");
        return;
    }
}

void Window::pollEvents() {
    glfwPollEvents();
}

GLFWwindow* Window::getGLFWWindow() const {
    return m_window;
}

void Window::setTitle(const std::string& title) {
    m_title = title;
    if (m_window) {
        glfwSetWindowTitle(m_window, m_title.c_str());
    }
    else {
        spdlog::error("Window is not initialized. Cannot set title.");
    }
}

void Window::setResizable(bool resizable) {
    m_resizable = resizable;
    if (m_window) {
        glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, m_resizable ? GLFW_TRUE : GLFW_FALSE);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set resizable attribute.");
    }
}

void Window::setSize(int width, int height) {
    m_width = width;
    m_height = height;
    if (m_window) {
        glfwSetWindowSize(m_window, m_width, m_height);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set size.");
    }
}

void Window::getSize(int& width, int& height) const {
    if (m_window) {
        glfwGetWindowSize(m_window, &width, &height);
    }
    else {
        spdlog::error("Window is not initialized. Cannot get size.");
        width = m_width;
        height = m_height;
    }
}


void Window::setFramebufferSizeCallback(FramebufferSizeCallback callback) {
    m_framebufferSizeCallback = callback;
    if (m_window) {
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set framebuffer size callback.");
    }
}


bool Window::isKeyPressed(int key) const {
    if (m_window) {
        return glfwGetKey(m_window, key) == GLFW_PRESS;
    }
    else {
        spdlog::error("Window is not initialized. Cannot check key state.");
        return false;
    }
}

bool Window::isMouseButtonPressed(int button) const {
    if (m_window) {
        return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
    }
    else {
        spdlog::error("Window is not initialized. Cannot check mouse button state.");
        return false;
    }
}


void Window::getMousePosition(double& x, double& y) const {
    if (m_window) {
        glfwGetCursorPos(m_window, &x, &y);
    }
    else {
        spdlog::error("Window is not initialized. Cannot get mouse position.");
        x = 0.0;
        y = 0.0;
    }
}


void Window::setMousePosition(double x, double y) {
    if (m_window) {
        glfwSetCursorPos(m_window, x, y);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set mouse position.");
    }
}


void Window::setMouseCursorVisible(bool visible) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set mouse cursor visibility.");
    }
}

void Window::setMouseCursorDisabled(bool disabled) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, disabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    else {
        spdlog::error("Window is not initialized. Cannot set mouse cursor disabled state.");
    }
}


Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
}


void Window::init(bool resizable) {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    // Set callbacks
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPositionCallback);
}


void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        win->m_width = width;
        win->m_height = height;
        if (win->m_framebufferSizeCallback) {
            win->m_framebufferSizeCallback(win, width, height);
        }
    }
}


void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ([[maybe_unused]] auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            // Handle key press events here if needed
            // spdlog::info("Key pressed: {} (scancode: {}, action: {}, mods: {})", key, scancode, action, mods);
        }
        else if (action == GLFW_RELEASE) {
            // Handle key release events here if needed
            // spdlog::info("Key released: {} (scancode: {}, action: {}, mods: {})", key, scancode, action, mods);
        }
    }
    else {
        spdlog::error("Window user pointer is null in key callback.");
    }
}


void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if ([[maybe_unused]] auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        if (action == GLFW_PRESS) {
            // Handle mouse button press events here if needed
            // spdlog::info("Mouse button pressed: {} (action: {}, mods: {})", button, action, mods);
        }
        else if (action == GLFW_RELEASE) {
            // Handle mouse button release events here if needed
            // spdlog::info("Mouse button released: {} (action: {}, mods: {})", button, action, mods);
        }
    }
    else {
        spdlog::error("Window user pointer is null in mouse button callback.");
    }
}

void Window::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if ([[maybe_unused]] auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        // Handle cursor position updates here if needed
        // spdlog::info("Cursor position updated: ({}, {})", xpos, ypos);
    }
    else {
        spdlog::error("Window user pointer is null in cursor position callback.");
    }
}

