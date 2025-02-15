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

        // normal drawing pass
        quadShader.use();
        quad.draw();
        quadShader.stopUsing();
    }

    void Renderer::initShaders()
    {
        // Ray Tracer Shader Program
        std::vector<Shader> ptShaders;
        Shader computeShader(config::shadersDir + "compute.glsl", GL_COMPUTE_SHADER);
        ptShaders.push_back(std::move(computeShader));
        rtShader.link(ptShaders);

        // Output Shader Program
        std::vector<Shader> screenShaders;
        Shader screenVS(config::shadersDir + "output.vert", GL_VERTEX_SHADER);
        Shader screenFS(config::shadersDir + "output.frag", GL_FRAGMENT_SHADER);
        screenShaders.push_back(std::move(screenVS));
        screenShaders.push_back(std::move(screenFS));
        quadShader.link(screenShaders);

        // Set values of uniforms
        rtShader.use();
        updateCameraUniforms();
        rtShader.setUniformInt("cubemap", TexImgUnits::cubemap);

        // for testing ------------------
        GLuint normalMapImgUnit = GL_TEXTURE2;
        rtShader.setUniformInt("wallNormalMap", 2);
        GLuint albedoMapImgUnit = GL_TEXTURE3;
        rtShader.setUniformInt("wallAlbedoMap", 3);

        glActiveTexture(normalMapImgUnit);
        scene.wallNormalMap.load("wallNormalMap.jpg", false);

        glActiveTexture(albedoMapImgUnit);
        scene.wallAlbedoMap.load("wallAlbedoMap.jpg", true);

        rtShader.setUniformVec3("wall.t1.v0.pos", scene.t1.v0.pos);
        rtShader.setUniformVec2("wall.t1.v0.uv", scene.t1.v0.uv);
        
        rtShader.setUniformVec3("wall.t1.v1.pos", scene.t1.v1.pos);
        rtShader.setUniformVec2("wall.t1.v1.uv", scene.t1.v1.uv);
        
        rtShader.setUniformVec3("wall.t1.v2.pos", scene.t1.v2.pos);
        rtShader.setUniformVec2("wall.t1.v2.uv", scene.t1.v2.uv);

        rtShader.setUniformMat3("wall.t1.TBN", scene.t1.calculateTBN());

        rtShader.setUniformVec3("wall.t2.v0.pos", scene.t2.v0.pos);
        rtShader.setUniformVec2("wall.t2.v0.uv", scene.t2.v0.uv);
        
        rtShader.setUniformVec3("wall.t2.v1.pos", scene.t2.v1.pos);
        rtShader.setUniformVec2("wall.t2.v1.uv", scene.t2.v1.uv);

        rtShader.setUniformVec3("wall.t2.v2.pos", scene.t2.v2.pos);
        rtShader.setUniformVec2("wall.t2.v2.uv", scene.t2.v2.uv);

        rtShader.setUniformMat3("wall.t2.TBN", scene.t2.calculateTBN());

        rtShader.setUniformVec3("pLight.position", scene.pLight.getPosition());
        rtShader.setUniformVec3("pLight.emission", scene.pLight.getEmission());

        rtShader.setUniformVec3("sphere.center", scene.sphere.getCenter());
        rtShader.setUniformFloat("sphere.radius", scene.sphere.getRadius());
        // ------------------------------

        rtShader.stopUsing();

        quadShader.use();
        quadShader.setUniformInt("quadTexture", TexImgUnits::outputTex);
        quadShader.stopUsing();

        // Load cubemap for compute shader
        glActiveTexture(GL_TEXTURE0 + TexImgUnits::cubemap);
        scene.cubemap.load();

        // Set up output texture
        initOutputTexture();
    }

    void Renderer::initOutputTexture()
    {
        glActiveTexture(GL_TEXTURE0 + TexImgUnits::outputTex);
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
        Viewport viewport = scene.camera.calculateViewport();
        rtShader.setUniformVec3("camera.position", scene.camera.getPosition());
        rtShader.setUniformVec3("viewport.horiz", viewport.horiz);
        rtShader.setUniformVec3("viewport.vert", viewport.vert);
        rtShader.setUniformVec3("viewport.pixel00", viewport.pixel00);
        rtShader.setUniformVec3("viewport.deltaHoriz", viewport.deltaHoriz);
        rtShader.setUniformVec3("viewport.deltaVert", viewport.deltaVert);
    }

    Renderer::Renderer(Renderer&& other) noexcept
        : scene(std::move(other.scene))
        , rtShader(std::move(other.rtShader))
        , quadShader(std::move(other.quadShader))
        , quad(std::move(other.quad))
        , outputTex{other.outputTex}
    {
        other.outputTex = 0;
    }

    Renderer& Renderer::operator=(Renderer&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            // outputTex is now 0
            std::swap(outputTex, other.outputTex);

            scene = std::move(other.scene);
            rtShader = std::move(other.rtShader);
            quadShader = std::move(other.quadShader);
            quad = std::move(other.quad);
        }
        return *this;
    }

    void Renderer::release()
    {
        glDeleteTextures(1, &outputTex);
        outputTex = 0;
    }

    Renderer::~Renderer()
    {
        release();
    }
}