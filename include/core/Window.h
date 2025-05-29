#pragma once
#include "pch.h"
class Window;


class Window {
public:
    Window(int width, int height, const std::string& title, bool resizable = true);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getGLFWWindow() const;
    void setTitle(const std::string& title);
    void setResizable(bool resizable);
    void setSize(int width, int height);
    void getSize(int& width, int& height) const;

    using FramebufferSizeCallback = std::function<void(Window*, int, int)>;
    void setFramebufferSizeCallback(FramebufferSizeCallback callback);

    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    void getMousePosition(double& x, double& y) const;
    void setMousePosition(double x, double y);
    void setMouseCursorVisible(bool visible);
    void setMouseCursorDisabled(bool disabled);

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    bool m_resizable;
    FramebufferSizeCallback m_framebufferSizeCallback;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

    void init(bool resizable);


};