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

    const std::string Renderer::quadTexName = "quadTexture";

    Renderer::Renderer(Scene scene)
        : scene(std::move(scene))
        , frameNumber(0)
    {
        if (!checkComputeLimits()) {
            throw new BoxOfSunlightError("The Compute Shader can't"
                " render the output image given its current size.");
        }
        glGenBuffers(1, &trianglesBuf);
        glGenBuffers(1, &spheresBuf);
        initShaders();
    }

    void Renderer::render()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // dispatch compute shader work groups, one for each pixel
        compShader.use();
        compShader.setUniformUnsignedInt("frameNumber", frameNumber);
        frameNumber++;
        glDispatchCompute((GLuint)config::windowWidth, (GLuint)config::windowHeight, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        compShader.stopUsing();

        // normal drawing pass
        quadShader.use();
        quad.draw();
        quadShader.stopUsing();
    }

    void Renderer::initShaders()
    {
        // ------------------  compute shader setup  ------------------
        // Compute Shader Program (based on ray tracing)
        std::vector<Shader> rtShaders;
        Shader computeShader(config::shadersDir + "raytrace.glsl", GL_COMPUTE_SHADER);
        rtShaders.push_back(std::move(computeShader));
        compShader.link(rtShaders);

        // point light (for testing)
        compShader.use();
        compShader.setUniformVec3("pLight.position", scene.pLight.position);
        compShader.setUniformVec3("pLight.emission", scene.pLight.emission);
        compShader.stopUsing();

        // random numbers (test)
        std::vector<GLuint> seeds(config::windowWidth * config::windowHeight);
        for (unsigned int j = 0; j < config::windowHeight; j++) {
            for (unsigned int i = 0; i < config::windowWidth; i++) {
                seeds[j * config::windowWidth + i] = rand();
            }
        }
        GLuint rngState;
        glGenBuffers(1, &rngState);
        passDataToSSBO(rngState, 3, seeds.size() * sizeof(GLuint), seeds.data());

        initCameraUniforms();
        initAllCompShaderTextures();
        
        // Pass triangle data
        passDataToSSBO(trianglesBuf, 1, scene.triangles.size() * sizeof(Triangle), scene.triangles.data());

        // Pass sphere data  
        passDataToSSBO(spheresBuf, 2, scene.spheres.size() * sizeof(Sphere), scene.spheres.data());
        // ------------------------------------------------------------
        // ------------------  output shader setup  -------------------
        // Output Shader Program (based on rasterization)
        std::vector<Shader> outputShaders;
        Shader outputVS(config::shadersDir + "quad.vert", GL_VERTEX_SHADER);
        Shader outputFS(config::shadersDir + "quad.frag", GL_FRAGMENT_SHADER);
        outputShaders.push_back(std::move(outputVS));
        outputShaders.push_back(std::move(outputFS));
        quadShader.link(outputShaders);
        
        initQuadTexture();
        // ------------------------------------------------------------
    }

    void Renderer::initAllCompShaderTextures()
    {
        compShader.use();

        /*
        // cubemap
        compShader.setUniformInt(CompShaderTexNames[cubemapImgUnit], cubemapImgUnit);
        glActiveTexture(GL_TEXTURE0 + cubemapImgUnit);
        scene.cubemap.load();
        */

        // albedo map
        initCompShader2DTex(scene.albedoMap, albedoMapImgUnit);
        // normal map
        initCompShader2DTex(scene.normalMap, normalMapImgUnit);
        // metallic map
        initCompShader2DTex(scene.metallicMap, metallicMapImgUnit);
        // roughness map
        initCompShader2DTex(scene.roughnessMap, roughnessMapImgUnit);

        compShader.stopUsing();
    }

    void Renderer::initCompShader2DTex(const Texture& tex, CompShaderTexImgUnits imgUnit)
    {
        compShader.setUniformInt(CompShaderTexNames[imgUnit], imgUnit);
        glActiveTexture(GL_TEXTURE0 + imgUnit);
        tex.load();
    }

    void Renderer::passDataToSSBO(GLuint buffer, GLuint index, GLsizeiptr size, const void* data)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void Renderer::initQuadTexture()
    {
        quadShader.use();
        quadShader.setUniformInt("quadTexture", quadTexImgUnit);
        quadShader.stopUsing();
        compShader.use();
        compShader.setUniformInt("srcImage", srcImgUnit);
        compShader.setUniformInt("dstImage", dstImgUnit);
        compShader.stopUsing();
        glActiveTexture(GL_TEXTURE0 + quadTexImgUnit);
        glGenTextures(1, &quadTexObj);
        glBindTexture(GL_TEXTURE_2D, quadTexObj);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config::windowWidth, config::windowHeight,
            0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindImageTexture(srcImgUnit, quadTexObj, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);   
        glBindImageTexture(dstImgUnit, quadTexObj, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
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
   
    void Renderer::initCameraUniforms()
    {
        compShader.use();
        Viewport viewport = scene.camera.calculateViewport();
        compShader.setUniformVec3("camera.position", scene.camera.getPosition());
        compShader.setUniformVec3("camera.viewport.horiz", viewport.horiz);
        compShader.setUniformVec3("camera.viewport.vert", viewport.vert);
        compShader.setUniformVec3("camera.viewport.pixel00", viewport.pixel00);
        compShader.setUniformVec3("camera.viewport.deltaHoriz", viewport.deltaHoriz);
        compShader.setUniformVec3("camera.viewport.deltaVert", viewport.deltaVert);
        compShader.stopUsing();
    }

    Renderer::Renderer(Renderer&& other) noexcept
        : scene(std::move(other.scene))
        , compShader(std::move(other.compShader))
        , quadShader(std::move(other.quadShader))
        , trianglesBuf(other.trianglesBuf)
        , spheresBuf(other.spheresBuf)
        , quad(std::move(other.quad))
        , quadTexObj{other.quadTexObj}
        , frameNumber(0)
    {
        other.quadTexObj = 0;
    }

    Renderer& Renderer::operator=(Renderer&& other) noexcept
    {
        // check for self-assignment
        if (this != &other)
        {
            release();
            // OpenGL objects are now 0
            std::swap(trianglesBuf, other.trianglesBuf);
            std::swap(spheresBuf, other.spheresBuf);
            std::swap(quadTexObj, other.quadTexObj);

            scene = std::move(other.scene);
            compShader = std::move(other.compShader);
            quadShader = std::move(other.quadShader);
            quad = std::move(other.quad);
            frameNumber = other.frameNumber;
        }
        return *this;
    }

    void Renderer::release()
    {
        glDeleteBuffers(1, &trianglesBuf);
        glDeleteBuffers(1, &spheresBuf);
        glDeleteTextures(1, &quadTexObj);
        quadTexObj = 0;
    }

    Renderer::~Renderer()
    {
        release();
    }
}