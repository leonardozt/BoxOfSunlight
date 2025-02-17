#include "Window.h"

Window::Window(int width, int height, const std::string& title)
{
    windowPtr = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    glfwMakeContextCurrent(windowPtr);
    // Swap the buffers immediately when glfwSwapBuffers() is called
    glfwSwapInterval(0);
}

Window::Window(Window&& other) noexcept
    : windowPtr{other.windowPtr}
{
    other.windowPtr = nullptr;
}

Window& Window::operator=(Window&& other) noexcept
{
    // check for self-assignment
    if (this != &other)
    {
        release();
        // windowPtr is now nullptr
        std::swap(windowPtr, other.windowPtr);
    }
    return *this;
}

void Window::update()
{
    glfwSwapBuffers(windowPtr);
}

void Window::release()
{
    if (windowPtr != nullptr) {
        glfwDestroyWindow(windowPtr);
    }
}

Window::~Window()
{
    release();
}