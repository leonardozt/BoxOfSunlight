#include "Renderer.h"

namespace BOSL
{ 
    Renderer::Renderer()
        : outputTex(0)
        , initialized(false)
        , camera(Camera(config::cameraStartPos,
                        config::cameraLookAt,
                        config::cameraVFOV))
        , cubemapImgUnit(0)
        , outputTexImgUnit(0)
    { }

    void Renderer::init()
    {
        initGL();
        initShaders();

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
        ptShader.use();
        glDispatchCompute((GLuint)config::windowWidth, (GLuint)config::windowHeight, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        ptShader.use();

        // Update camera position (for testing)
        ptShader.use();
        cameraDegree += 0.3f;
        glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f),
            glm::radians(cameraDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 newPos = glm::vec3(rotationMat * glm::vec4(config::cameraStartPos, 1.0f));
        camera.setPosition(newPos);
        updateCameraUniforms();
        ptShader.stopUsing();

        // normal drawing pass
        quadShader.use();
        quad.draw(quadShader);
        quadShader.stopUsing();
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
        // Print limitations for Compute Shaders
        debug::printComputeLimits();
    }

    void Renderer::initShaders()
    {
        cubemapImgUnit = GL_TEXTURE0;
        outputTexImgUnit = GL_TEXTURE1;

        // Path Tracer Shader Program
        ptShader.init();
        std::vector<Shader> ptShaders;
        Shader computeShader(config::shadersDir + "compute.glsl", GL_COMPUTE_SHADER);
        ptShaders.push_back(computeShader);
        ptShader.link(ptShaders);

        // Output Shader Program
        quadShader.init();
        std::vector<Shader> screenShaders;
        Shader screenVS(config::shadersDir + "screen.vert", GL_VERTEX_SHADER);
        Shader screenFS(config::shadersDir + "screen.frag", GL_FRAGMENT_SHADER);
        screenShaders.push_back(screenVS); screenShaders.push_back(screenFS);
        quadShader.link(screenShaders);

        // Set values of uniforms
        ptShader.use();
        updateCameraUniforms();
        ptShader.setUniformInt("cubemap", 0);
        ptShader.stopUsing();
        quadShader.use();
        quadShader.setUniformInt("quadTexture", 1);
        quadShader.stopUsing();

        // Setup cubemap for compute shader
        cubemap.init(cubemapImgUnit);

        // Setup screen quad
        quad.init();
        // Setup output texture
        initOutputTexture();
    }

    void Renderer::initOutputTexture()
    {
        glActiveTexture(outputTexImgUnit);
        glGenTextures(1, &outputTex);
        glBindTexture(GL_TEXTURE_2D, outputTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config::windowWidth, config::windowHeight,
            0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    }

    bool Renderer::checkComputeLimits()
    {
        GLint workGroupCount[2];
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
   
    void Renderer::updateCameraUniforms()
    {
        Viewport viewport = camera.getViewport();
        ptShader.setUniformVec3("camera.position", camera.getPosition());
        ptShader.setUniformVec3("viewport.horiz", viewport.horiz);
        ptShader.setUniformVec3("viewport.vert", viewport.vert);
        ptShader.setUniformVec3("viewport.pixel00", viewport.pixel00);
        ptShader.setUniformVec3("viewport.deltaHoriz", viewport.deltaHoriz);
        ptShader.setUniformVec3("viewport.deltaVert", viewport.deltaVert);
    }
}