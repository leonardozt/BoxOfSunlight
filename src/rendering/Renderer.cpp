#include "Renderer.h"

namespace BOSL
{
    void initGL()
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

    Renderer::Renderer(Scene scene)
        : scene(std::move(scene))
    {
        initShaders();

        if (!checkComputeLimits()) {
            throw new BoxOfSunlightError("The Compute Shader can't"
                " render the output image given its current size.");
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
        Shader computeShader(config::shadersDir + "raytrace.glsl", GL_COMPUTE_SHADER);
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
        rtShader.setUniformInt("albedoMap", TexImgUnits::albedoMap);
        rtShader.setUniformInt("normalMap", TexImgUnits::normalMap);
        
        glActiveTexture(GL_TEXTURE0 + TexImgUnits::albedoMap);
        scene.albedoMap.load(scene.albedoMapImgPath, true);
        glActiveTexture(GL_TEXTURE0 + TexImgUnits::normalMap);
        scene.normalMap.load(scene.normalMapImgPath, false);
        
        // Pass triangle data
        GLuint trianglesBuf; // SSBO for triangles
        glGenBuffers(1, &trianglesBuf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, scene.triangles.size() * sizeof(Triangle),
            scene.triangles.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesBuf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  
        // Pass sphere data
        GLuint spheresBuf; // SSBO for spheres
        glGenBuffers(1, &spheresBuf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, spheresBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, scene.spheres.size() * sizeof(Sphere),
            scene.spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, spheresBuf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        rtShader.setUniformVec3("pLight.position", scene.pLight.position);
        rtShader.setUniformVec3("pLight.emission", scene.pLight.emission);

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
        rtShader.setUniformVec3("camera.viewport.horiz", viewport.horiz);
        rtShader.setUniformVec3("camera.viewport.vert", viewport.vert);
        rtShader.setUniformVec3("camera.viewport.pixel00", viewport.pixel00);
        rtShader.setUniformVec3("camera.viewport.deltaHoriz", viewport.deltaHoriz);
        rtShader.setUniformVec3("camera.viewport.deltaVert", viewport.deltaVert);
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