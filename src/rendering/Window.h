#pragma once

#include <GLFW/glfw3.h>
#include <string>

#include "..\utils\BoxOfSunlightError.h"

// This class is used to create and manage a glfw window.
// glfw initialization and window hints need to be done before creation.
class Window {
public:
    // Creation makes the OpenGL context of the window current
    Window(int width, int height, const std::string& title);

    ~Window();

    // Delete the copy constructor/assignment
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // move constructor
    Window(Window&& other) noexcept;
    // move assignment
    Window& operator=(Window&& other) noexcept;

    // Buffer swap
    void update();

    int shouldClose() { return glfwWindowShouldClose(windowPtr); }

private:
    GLFWwindow* windowPtr;

    void release();
};