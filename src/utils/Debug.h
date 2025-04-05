#pragma once

#include "Config.h"
#include "BoxOfSunlightError.h"

namespace BOSL
{
    namespace debug
    {
        // Callback function for GLFW errors
        void glfwErrorCallback(int error, const char* description);

        // Callback function that receives debugging messages from OpenGL
        void GLAPIENTRY debugMessageCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam);
        
        // Prints to console the system's limitations for Compute Shader Workgroups.
        void printComputeLimits();
    }
}