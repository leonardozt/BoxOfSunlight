#include <GL/glew.h>
#include<GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Window.h"
#include "BoxOfSunlightError.h"
#include "Shader.h"
#include "Program.h"

namespace config
{
    // Configuration Values
    const int windowWidth = 800;
    const int windowHeight = 600;
    const std::string windowTitle = "BoxOfSunlight";
    const std::string shadersDir = "..\\..\\..\\src\\shaders\\";
    const std::string vertexShaderPath = shadersDir + "VertexShader.vert";
    const std::string fragmentShaderPath = shadersDir + "FragmentShader.frag";
}

// Callback function that receives debugging messages from OpenGL
void GLAPIENTRY debugMessageCallback(GLenum source,
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
        throw BoxOfSunlightError("Error due to high debug severity");
    }
}

// Initializes GLEW and sets the needed OpenGL settings
static void initGL()
{
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        throw BoxOfSunlightError("GLEW failed to initialize");
    }
    // Enable Debug Mode
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessageCallback, 0);
    // Set Viewport
    glViewport(0, 0, config::windowWidth, config::windowHeight);

    std::vector<BOSL::Shader> shaders;
    shaders.push_back(BOSL::Shader(config::vertexShaderPath, GL_VERTEX_SHADER));
    shaders.push_back(BOSL::Shader(config::fragmentShaderPath, GL_FRAGMENT_SHADER));
    BOSL::Program shaderProgram(shaders);

    shaderProgram.use();
}

static void initGeometry()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int main()
{
    try {    
        Window window = Window(config::windowWidth, config::windowHeight, config::windowTitle);
        initGL();
        initGeometry();

        while (!window.shouldClose())
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            window.update();
        }
        return EXIT_SUCCESS;

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}