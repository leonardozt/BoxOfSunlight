#pragma once
#include<GLFW/glfw3.h>
#include <string>
#include <iostream>

#include "BoxOfSunlightError.h"

class Window {
public:
    Window(int width, int height, const std::string& title);

    ~Window();

    void update();

    int shouldClose() { return glfwWindowShouldClose(glfwWindow); }

private:
    GLFWwindow* glfwWindow;

    static void glfwErrorCallback(int error, const char* description);
};