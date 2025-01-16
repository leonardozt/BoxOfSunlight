#include <GL/glew.h>
#include<GLFW/glfw3.h>

#include <iostream>

#include "Window.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "BoxOfSunlight"

// Callback function that receives debugging messages from OpenGL
static void GLAPIENTRY debugMessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        throw std::exception();
    }
}

static void initGL()
{
    if (glewInit() != GLEW_OK) {
        throw std::exception();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(debugMessageCallback, 0);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

static void updateGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
    try {    
        Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
        initGL();

        while (!window.shouldClose())
        {
            updateGL();
            window.update();
        }
        return EXIT_SUCCESS;

    } catch(std::exception) {
        return EXIT_FAILURE;
    }
}