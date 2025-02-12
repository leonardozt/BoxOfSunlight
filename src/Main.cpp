#include "Config.h"
#include "Window.h"
#include "BoxOfSunlightError.h"
#include "Renderer.h"
#include "Debug.h"

// Callback function for GLFW errors
void glfwErrorCallback(int error, const char* description)
{
    std::string mess = "GLFW Error: " + std::string(description) + "\n";
    throw BoxOfSunlightError(mess);
}

// Initializes GLEW and sets the needed OpenGL settings
void initGL()
{
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        throw BoxOfSunlightError("GLEW failed to initialize");
    }
    // Enable Debug Mode
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(BOSL::debug::debugMessageCallback, 0);
    // Set Viewport
    glViewport(0, 0, BOSL::config::windowWidth, BOSL::config::windowHeight);
    // Print limitations for Compute Shaders
    BOSL::debug::printComputeLimits();
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);

    int exitStatus = EXIT_SUCCESS;

    try {    
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        Window window(
            BOSL::config::windowWidth,
            BOSL::config::windowHeight,
            BOSL::config::windowTitle
        );
    
        initGL();
        
        BOSL::Scene scene;

        BOSL::Renderer renderer(std::move(scene));
        
        // timing 
        double deltaTime = 0.0f; // time between current frame and last frame
        double lastFrame = 0.0f; // time of last frame
        int fCounter = 0;

        while (!window.shouldClose())
        {
            renderer.render();
            window.update();
            glfwPollEvents();

            // Set frame time
            double currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            // Print only every 500 frames
            if (fCounter > 500) {
                std::cout << "FPS: " << 1 / deltaTime << std::endl;
                fCounter = 0;
            }
            else {
                fCounter++;
            }
        }        

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        exitStatus = EXIT_FAILURE;
    }

    glfwTerminate();
    return exitStatus;
}