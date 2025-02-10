#include <iostream>

#include "Config.h"
#include "Window.h"
#include "BoxOfSunlightError.h"
#include "Renderer.h"

int main()
{
    try {    
        Window window = Window(
            BOSL::config::windowWidth,
            BOSL::config::windowHeight,
            BOSL::config::windowTitle
        );
        
        BOSL::Renderer renderer;
        renderer.init();
        
        // timing 
        double deltaTime = 0.0f; // time between current frame and last frame
        double lastFrame = 0.0f; // time of last frame
        int fCounter = 0;
        
        while (!window.shouldClose())
        {
            renderer.render();
            window.update();

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
        return EXIT_SUCCESS;

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}