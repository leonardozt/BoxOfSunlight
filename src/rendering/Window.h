#pragma once

#include <GLFW/glfw3.h>
#include <string>

#include "..\utils\BoxOfSunlightError.h"

namespace BOSL {
    // initializes glfw and sets window hints.
    void initGLFW();

    // This class is used to create and manage a glfw window.
    // Call initGLFW() before you create Window objects.
    // After you are done with glfw, remember to call glfwTerminate()
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

        // To get a proper value, remember to periodically call glfwPollEvents()
        // in main loop.
        int shouldClose() { return glfwWindowShouldClose(windowPtr); }

    private:
        GLFWwindow* windowPtr;

        void release();
    };
}