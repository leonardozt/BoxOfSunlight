#include "Renderer.h"

namespace BOSL
{
    void initGL()
    {
        // Initialize GLEW
        if (glewInit() != GLEW_OK) {
            throw BoxOfSunlightError("GLEW failed to initialize");
        }        
        if (config::debugMode) {
            // Enable Debug Mode
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debug::debugMessageCallback, 0);
            // Print limitations for Compute Shader
            debug::printComputeLimits();        
        }
        // Set viewport
        glViewport(0, 0, config::windowWidth, config::windowHeight);
    }

    const std::array<std::string, Renderer::NUM_COMP_TEX_IMG_UNITS>
        Renderer::compShaderTexNames =
    {
        "cubemap",
        "albedoMap",
        "normalMap",
        "metallicMap",
        "roughnessMap"
    };
    const std::array<std::string, Renderer::NUM_COMP_IMG_UNITS>
        Renderer::compShaderImgNames =
    {
        "srcImage",
        "dstImage"
    };
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
        initAllShaders();
    }

    void Renderer::render()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        // Render image chunks, one at a time
        compShader.use();
        constexpr glm::uvec2 numChunks = config::numChunks;
        constexpr unsigned int chunkWidth = config::windowWidth / numChunks.x;
        constexpr unsigned int chunkHeight = config::windowHeight / numChunks.y;
        for (unsigned int j = 0; j < numChunks.y; j++) {
            for (unsigned int i = 0; i < numChunks.x; i++) {
                // set uniforms
                compShader.setUniformUnsignedInt("frameNumber", frameNumber);
                compShader.setUniformUnsignedIntVec2("chunkOffset",
                    glm::uvec2(chunkWidth*i, chunkHeight*j));

                // dispatch compute shader work groups, one for each pixel in chunk
                glDispatchCompute((GLuint)chunkWidth, (GLuint)chunkHeight, 1);
                // make sure writing to image has finished before read
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);   
            }
        }
        compShader.stopUsing();

        // send output texture to quad shader for second rendering pass
        quadShader.use();
        quad.draw();
        quadShader.stopUsing();

        frameNumber++;
    }

    void Renderer::initAllShaders()
    {
        initCompShader();
        initQuadShader();
        // quad texture is used by both shaders
        // (one writes on it, the other reads)
        initQuadTexture();
    }

    void Renderer::initCompShader()
    {
        std::vector<Shader> rtShaders;
        Shader computeShader(config::shadersDir + "\\raytrace.glsl", GL_COMPUTE_SHADER);
        rtShaders.push_back(std::move(computeShader));
        compShader.link(rtShaders);

        initCompShaderUniformBools();
        
        // Lighting
        compShader.use();
        if (!scene.useCubemap) {
            // point light
            compShader.setUniformVec3("pLight.position", scene.pLight.position);
            compShader.setUniformVec3("pLight.emission", scene.pLight.emission);
        } else {
            // number of hemisphere samples, used in the case of IBL with cubemap
            compShader.setUniformUnsignedInt("hemisphereSamples", scene.hemisphereSamples);
        }
        compShader.stopUsing();

        // random numbers
        std::vector<unsigned int> randomSeeds;
        for (unsigned int i = 0; i < config::windowWidth*config::windowHeight; i++) {
                randomSeeds.push_back(rand());
        }
        GLuint rngStateBuf;
        glGenBuffers(1, &rngStateBuf);
        passDataToSSBO(rngStateBuf, rngStateBufIdx,
            randomSeeds.size() * sizeof(GLuint), randomSeeds.data(), GL_DYNAMIC_DRAW);

        initCameraUniforms();
        initMaterialUniforms();
        initAllCompShaderTextures();
        
        // Pass triangle data
        passDataToSSBO(trianglesBuf, trianglesBufIdx,
            scene.triangles.size() * sizeof(Triangle), scene.triangles.data());
        // Pass sphere data  
        passDataToSSBO(spheresBuf, spheresBufIdx,
            scene.spheres.size() * sizeof(Sphere), scene.spheres.data());
    }

    void Renderer::initQuadShader()
    {
        std::vector<Shader> outputShaders;
        Shader outputVS(config::shadersDir + "\\quad.vert", GL_VERTEX_SHADER);
        Shader outputFS(config::shadersDir + "\\quad.frag", GL_FRAGMENT_SHADER);
        outputShaders.push_back(std::move(outputVS));
        outputShaders.push_back(std::move(outputFS));
        quadShader.link(outputShaders);

        quadShader.use();
        quadShader.setUniformFloat("exposure", scene.exposure);
        quadShader.stopUsing();
    }

    void Renderer::initAllCompShaderTextures()
    {
        compShader.use();
        // cubemap
        if (scene.useCubemap) {
            compShader.setUniformInt(compShaderTexNames.at(cubemapImgUnit),
                cubemapImgUnit);
            glActiveTexture(GL_TEXTURE0 + cubemapImgUnit);
            scene.cubemap.load();
        }
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
        compShader.setUniformInt(compShaderTexNames.at(imgUnit), imgUnit);
        glActiveTexture(GL_TEXTURE0 + imgUnit);
        tex.load();
    }

    void Renderer::passDataToSSBO(GLuint buffer, GLuint index,
        GLsizeiptr size, const void* data, GLenum usage)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void Renderer::initQuadTexture()
    {
        quadShader.use();
        quadShader.setUniformInt(quadTexName, quadTexImgUnit);
        quadShader.stopUsing();
        compShader.use();
        compShader.setUniformInt(compShaderImgNames.at(srcImgUnit), srcImgUnit);
        compShader.setUniformInt(compShaderImgNames.at(dstImgUnit), dstImgUnit);
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

        if (workGroupCount[0] < (int)(config::windowWidth/config::numChunks.x) ||
            workGroupCount[1] < (int)(config::windowHeight/config::numChunks.y)) {
            return false;
        }
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

    void Renderer::initMaterialUniforms()
    {
        compShader.use();
        Material material = scene.material;
        compShader.setUniformVec3("material.baseColor", material.baseColor);
        compShader.setUniformFloat("material.subsurface", material.subsurface);
        compShader.setUniformFloat("material.metallic", material.metallic);
        compShader.setUniformFloat("material.specularTint", material.specularTint);
        compShader.setUniformFloat("material.roughness", material.roughness);
        compShader.setUniformFloat("material.anisotropic", material.anisotropic);
        compShader.setUniformFloat("material.sheen", material.sheen);
        compShader.setUniformFloat("material.sheenTint", material.sheenTint);
        compShader.setUniformFloat("material.clearCoat", material.clearCoat);
        compShader.setUniformFloat("material.clearCoatGloss", material.clearCoatGloss);
        compShader.stopUsing();
    }

    void Renderer::initCompShaderUniformBools()
    {
        compShader.use();
        compShader.setUniformBool("useCubemap", scene.useCubemap);
        compShader.setUniformBool("useAlbedoMap", scene.useAlbedoMap);
        compShader.setUniformBool("useNormalMap", scene.useNormalMap);
        compShader.setUniformBool("useMetallicMap", scene.useMetallicMap);
        compShader.setUniformBool("useRoughnessMap", scene.useRoughnessMap);
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