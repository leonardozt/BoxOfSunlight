#include "Renderer.h"

namespace BOSL
{ 
    Renderer::Renderer() : outputTexture(0), initialized(false) { }

    void Renderer::init()
    {
        initGL();
        initShaders();
        initOutputTexture();
        quad.init();
        
        if (!checkComputeLimits()) {
            throw new BoxOfSunlightError("The Compute Shader can't"
                " render the output image with its current size.");
        }

        initialized = true;
    }

    void Renderer::render()
    {
        if (!initialized) {
            throw BoxOfSunlightError("Renderer was not initialized before use.");
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // dispatch compute shader work groups, one for each pixel
        pathTracerShader.use();
        glDispatchCompute((GLuint)config::windowWidth, (GLuint)config::windowHeight, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // normal drawing pass
        screenQuadShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, outputTexture);
        quad.draw(screenQuadShader);
    }

    void Renderer::initGL()
    {
        // Initialize GLEW
        if (glewInit() != GLEW_OK) {
            throw BoxOfSunlightError("GLEW failed to initialize");
        }
        // Enable Debug Mode
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug::debugMessageCallback, 0);
        // Set Viewport
        glViewport(0, 0, config::windowWidth, config::windowHeight);
        // Print Limitations for Compute Shaders
        debug::printComputeLimits();
    }

    void Renderer::initShaders()
    {
        pathTracerShader.init();
        screenQuadShader.init();

        std::vector<Shader> ptShaders;
        Shader computeShader(config::shadersDir + "compute.glsl", GL_COMPUTE_SHADER);
        ptShaders.push_back(computeShader);
        pathTracerShader.link(ptShaders);

        std::vector<Shader> screenShaders;
        Shader screenVS(config::shadersDir + "screen.vert", GL_VERTEX_SHADER);
        Shader screenFS(config::shadersDir + "screen.frag", GL_FRAGMENT_SHADER);
        screenShaders.push_back(screenVS); screenShaders.push_back(screenFS);
        screenQuadShader.link(screenShaders);
    }

    void Renderer::initOutputTexture()
    {
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &outputTexture);
        glBindTexture(GL_TEXTURE_2D, outputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config::windowWidth, config::windowHeight,
            0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    }

    bool Renderer::checkComputeLimits()
    {
        GLint workGroupCount[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);

        if (workGroupCount[0] < config::windowWidth ||
            workGroupCount[1] < config::windowHeight) {
            return false;
        }

        /*
        GLint workGroupSize[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
        */
        // TODO: check local size (within pixel) and number of invocations
        /*
        GLint workGroupInv;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
        */
        return true;
    }
}