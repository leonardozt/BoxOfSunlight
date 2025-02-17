#pragma once

#include "Config.h"
#include "BoxOfSunlightError.h"

namespace BOSL
{
    namespace debug
    {
        // Callback function that receives debugging messages from OpenGL
        void GLAPIENTRY debugMessageCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam);
        
        // Prints system's limitations for Compute Shader Workgroups to console,
        // Returns true if the output image can rendered, false otherwhise
        void printComputeLimits();
    }
}