#include "utils\Config.h"
#include "utils\BoxOfSunlightError.h"
#include "utils\Debug.h"
#include "rendering\Window.h"
#include "rendering\Renderer.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

BOSL::Scene prepareScene() {
    BOSL::Scene scene;

    // Wall object
    glm::vec4 p0(-1.0, -1.0, 1.0, 1.0);
    glm::vec4 p1(1.0, -1.0, 1.0, 1.0);
    glm::vec4 p2(1.0, 1.0, 1.0, 1.0);
    glm::vec4 p3(-1.0, 1.0, 1.0, 1.0);

    glm::vec2 uv0(0.0, 0.0);
    glm::vec2 uv1(1.0, 0.0);
    glm::vec2 uv2(1.0, 1.0);
    glm::vec2 uv3(0.0, 1.0);

    BOSL::Vertex v0{ p0, uv0 };
    BOSL::Vertex v1{ p1, uv1 };
    BOSL::Vertex v2{ p2, uv2 };
    BOSL::Vertex v3{ p3, uv3 };

    glm::mat3 TBN1 = BOSL::triangleTBN(v0, v1, v2);
    glm::mat3 TBN2 = BOSL::triangleTBN(v0, v2, v3);

    BOSL::Triangle t1{ v0, v1, v2, glm::mat4(TBN1)};
    BOSL::Triangle t2{ v0, v2, v3, glm::mat4(TBN2)};
    
    scene.triangles.push_back(t1);
    scene.triangles.push_back(t2);

    BOSL::PointLight pLight;
    pLight.position = glm::vec3(3.0f, 2.0f, 2.5f);
    pLight.emission = glm::vec3(1.0f, 1.0f, 1.0f);
    scene.pLight = pLight;
    
    //BOSL::Sphere sphere{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f };
    //scene.spheres.push_back(sphere);

    return scene;
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);

    int exitStatus = EXIT_SUCCESS;

    try {    
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        Window window(
            BOSL::config::windowWidth,
            BOSL::config::windowHeight,
            BOSL::config::windowTitle
        );
    
        initGL();
        
        BOSL::Scene scene = prepareScene();
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