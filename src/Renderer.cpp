#include "Renderer.h"

namespace BOSL
{ 
    Renderer::Renderer(Scene scene)
        : scene(std::move(scene))
    {
        initShaders();

        if (!checkComputeLimits()) {
            throw new BoxOfSunlightError("The Compute Shader can't"
                " render the output image with its current size.");
        }
    }

    void Renderer::render()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // dispatch compute shader work groups, one for each pixel
        rtShader.use();
        glDispatchCompute((GLuint)config::windowWidth, (GLuint)config::windowHeight, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        rtShader.stopUsing();

        // Update camera position (for testing)
        rtShader.use();
        cameraDegree += 0.3f;
        glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f),
            glm::radians(cameraDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 newPos = glm::vec3(rotationMat * glm::vec4(config::cameraStartPos, 1.0f));
        scene.camera.setPosition(newPos);
        updateCameraUniforms();
        rtShader.stopUsing();

        // normal drawing pass
        quadShader.use();
        quad.draw();
        quadShader.stopUsing();
    }

    void Renderer::initShaders()
    {
        cubemapImgUnit = GL_TEXTURE0;
        outputTexImgUnit = GL_TEXTURE1;

        // Ray Tracer Shader Program
        std::vector<Shader> ptShaders;
        Shader computeShader(config::shadersDir + "compute.glsl", GL_COMPUTE_SHADER);
        ptShaders.push_back(std::move(computeShader));
        rtShader.link(ptShaders);

        // Output Shader Program
        std::vector<Shader> screenShaders;
        Shader screenVS(config::shadersDir + "screen.vert", GL_VERTEX_SHADER);
        Shader screenFS(config::shadersDir + "screen.frag", GL_FRAGMENT_SHADER);
        screenShaders.push_back(std::move(screenVS));
        screenShaders.push_back(std::move(screenFS));
        quadShader.link(screenShaders);

        // Set values of uniforms
        rtShader.use();
        updateCameraUniforms();
        rtShader.setUniformInt("cubemap", 0);
        rtShader.stopUsing();
        quadShader.use();
        quadShader.setUniformInt("quadTexture", 1);
        quadShader.stopUsing();

        // Load cubemap for compute shader
        glActiveTexture(cubemapImgUnit);
        scene.cubemap.load();

        // Set up output texture
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
        Viewport viewport = scene.camera.getViewport();
        rtShader.setUniformVec3("camera.position", scene.camera.getPosition());
        rtShader.setUniformVec3("viewport.horiz", viewport.horiz);
        rtShader.setUniformVec3("viewport.vert", viewport.vert);
        rtShader.setUniformVec3("viewport.pixel00", viewport.pixel00);
        rtShader.setUniformVec3("viewport.deltaHoriz", viewport.deltaHoriz);
        rtShader.setUniformVec3("viewport.deltaVert", viewport.deltaVert);
    }
}