#include "Window.h"

// Callback function for GLFW errors
void Window::glfwErrorCallback(int error, const char* description)
{
    std::string mess = "GLFW Error: " + std::string(description) + "\n";
    throw BoxOfSunlightError(mess);
}

Window::Window(int width, int height, const std::string& title)
{
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    glfwMakeContextCurrent(glfwWindow);
}

void Window::update()
{
    glfwSwapBuffers(glfwWindow);
    glfwPollEvents();
}

Window::~Window()
{
    glfwDestroyWindow(glfwWindow);
}