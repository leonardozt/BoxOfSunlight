#include "utils\Config.h"
#include "utils\BoxOfSunlightError.h"
#include "utils\Debug.h"
#include "rendering\Window.h"
#include "rendering\Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader\tiny_obj_loader.h>

std::vector<BOSL::Triangle> loadObj(std::string objPath);
BOSL::Scene convergenceTest();
BOSL::Scene spheres();
BOSL::Scene threeLobes();
BOSL::Scene goldenHour();
BOSL::Scene plane();
BOSL::Scene sheen();
BOSL::Scene elephant();
BOSL::Scene suzanne();

int main()
{
    int exitStatus = EXIT_SUCCESS;

    glfwSetErrorCallback(BOSL::debug::glfwErrorCallback);

    try {    
        BOSL::initGLFW();
        BOSL::Window window(
            BOSL::config::windowWidth,
            BOSL::config::windowHeight,
            BOSL::config::windowTitle
        );
    
        BOSL::initGL();

        // Set up scene                
        BOSL::Scene scene = convergenceTest();                
        
        // Create renderer object
        BOSL::Renderer renderer(std::move(scene));

        unsigned int fCounter = 0; // frame counter               
        while (!window.shouldClose())
        {
            double startTime = glfwGetTime();

            renderer.render();
            window.update(); 

            double endTime = glfwGetTime();

            glfwPollEvents();    
            fCounter++;        

            double deltaTime = endTime - startTime;
            std::cout
                << "frame #"
                << fCounter
                << " rendering time: "
                << (int)deltaTime / 60 << "m "
                << (int)deltaTime % 60 << "s\n";        
        }

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        exitStatus = EXIT_FAILURE;
    }

    glfwTerminate();
    return exitStatus;
}

std::vector<BOSL::Triangle> loadObj(std::string objPath)
{
    std::vector<BOSL::Triangle> triangles;

    size_t pos = objPath.find_last_of("\\");
    std::string mtlBaseDir = objPath.substr(0, pos + 1);

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials,
        &warn, &err, objPath.c_str(), mtlBaseDir.c_str());

    if (!warn.empty()) {
        std::cerr << "tinyobjloader warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "tinyobjloader error: " << err << std::endl;
    }
    if (!ret) {
        throw BoxOfSunlightError("tinyobjloader failed to load .obj file");
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        // Each shape contains a mesh
        tinyobj::mesh_t& mesh = shapes[s].mesh;
        // Loop over faces (triangles)
        size_t index_offset = 0;
        for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {

            size_t fv = size_t(mesh.num_face_vertices[f]);
            if (fv != 3) {
                throw BoxOfSunlightError("Error: obj model cotains "
                    "a face that is not a triangle.");
            }

            // Loop over vertices in the face (assumed to be 3)
            BOSL::Vertex vertices[3];
            for (size_t v = 0; v < 3; v++) {
                // access to vertex
                tinyobj::index_t idx = mesh.indices[index_offset + v];

                tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];
                glm::vec4 position(vx, vy, vz, 1.0f);

                // Check if `texcoord_index` is negative. negative = no texcoord data
                if (idx.texcoord_index < 0) {
                    throw BoxOfSunlightError("texture coordinates not found for vertex in .obj file");
                }

                tinyobj::real_t tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
                tinyobj::real_t ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
                glm::vec2 texCoords(tx, ty);

                vertices[v] = BOSL::Vertex{ position, texCoords };
            }
            triangles.push_back(BOSL::Triangle(vertices[0], vertices[1], vertices[2]));

            index_offset += 3;
        }
    }
    return triangles;
}

BOSL::Scene convergenceTest()
{
    BOSL::Scene scene;
    
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,0.0f,1.0f), 1.0f });

    scene.useCubemap = true;
    scene.hemisphereSamples = 1024;
    scene.exposure = 3.0f;

    scene.camera.setPosition(glm::vec3(0.0f,0.0f,2.3f));
        
    scene.material.baseColor = glm::vec3(0.10f, 0.22f, 0.80f);
    scene.material.metallic = 1.0f;
    scene.material.roughness = 0.1f;

    return scene;
}

BOSL::Scene spheres()
{
    BOSL::Scene scene;

    scene.pLight.position = glm::vec3(2.0f, 2.0f, 10.0f);
    scene.pLight.emission = glm::vec3(3.0f);
    scene.exposure = 1.0f;
    
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 3.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-5.0f, -4.3f, -5.0f, 1.0f), 2.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(4.0f, -1.0f, -1.5f, 1.0f), 1.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-3.5f, 3.5f, -1.0f, 1.0f), 0.5f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(3.8f, 3.3f, -2.0f, 1.0f), 2.0f });

    scene.camera.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));

    scene.material.baseColor = glm::vec3(0.6f, 0.6f, 0.6f);
    scene.material.metallic = 1.0f;
    scene.material.anisotropic = 1.0f;
    scene.material.roughness = 0.3f;

    return scene;
}

BOSL::Scene threeLobes()
{
    BOSL::Scene scene;
    
    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 3.0f;

    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,2.5f,1.0f), 1.0f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,0.0f,1.0f), 1.0f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,-2.5f,1.0f), 1.0f });

    scene.camera.setPosition(glm::vec3(5.8f, 0.0f, 0.0f));
    scene.camera.setVFOV(25.0f);

    scene.albedoMap.setImgFilePath(BOSL::config::texturesDir + "\\RuinedBricks\\albedo.jpg");
    scene.useAlbedoMap = true;
    scene.normalMap.setImgFilePath(BOSL::config::texturesDir + "\\RuinedBricks\\normalDX.jpg");
    scene.useNormalMap = true;
    scene.roughnessMap.setImgFilePath(BOSL::config::texturesDir + "\\RuinedBricks\\roughness.jpg");
    scene.useRoughnessMap = true;

    return scene;
}

BOSL::Scene goldenHour()
{
    BOSL::Scene scene;

    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,0.0f,1.0f), 1.0f });

    scene.camera.setPosition(glm::vec3(3.5f, 0.0f, 1.8f));

    scene.camera.setVFOV(45.0f);

    scene.cubemap.setDirName("SunriseCubemap");
    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 2.0f;
    
    scene.material.specular = 1.0f;
    scene.material.specularTint = 1.0f;
    scene.material.anisotropic = 1.0f;

    scene.material.baseColor = glm::vec3(0.73f, 0.56f, 0.3f);

    scene.material.metallic = 1.0f;
    scene.material.roughness = 0.3f;

    return scene;
}

BOSL::Scene plane()
{
    BOSL::Scene scene;

    BOSL::Vertex v0{ glm::vec4(-10.0f, 0.0f, 10.0f, 1.0f), glm::vec2(0.0f, 0.0f) };
    BOSL::Vertex v1{ glm::vec4( 10.0f, 0.0f, 10.0f, 1.0f), glm::vec2(1.0f, 0.0f) };
    BOSL::Vertex v2{ glm::vec4( 10.0f, 0.0f,-10.0f, 1.0f), glm::vec2(1.1f, 1.1f) };
    BOSL::Vertex v3{ glm::vec4(-10.0f, 0.0f,-10.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

    scene.triangles.push_back(BOSL::Triangle(v0, v1, v2));
    scene.triangles.push_back(BOSL::Triangle(v0, v2, v3));

    scene.cubemap.setDirName("SunriseCubemap");
    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 2.0f;

    scene.camera.setPosition(glm::vec3(0.0f, 0.1f, -10.2f));
    //scene.camera.setPosition(glm::vec3(0.0f, 20.0f, -20.0f));
    
    scene.camera.setVFOV(40.0f);

    scene.material.baseColor = glm::vec3(0.95f, 0.20f, 0.05f);
    scene.material.roughness = 0.0f;

    return scene;
}

BOSL::Scene sheen()
{
    BOSL::Scene scene;

    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 3.0f;

    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f,0.0f,0.0f,1.0f), 1.0f });

    scene.camera.setPosition(glm::vec3(0.0f, 0.0f, -3.0f));

    scene.albedoMap.setImgFilePath(BOSL::config::texturesDir + "\\fabric\\albedo.jpg");
    scene.useAlbedoMap = true;
    scene.normalMap.setImgFilePath(BOSL::config::texturesDir + "\\fabric\\normalDX.jpg");
    scene.useNormalMap = true;
    scene.roughnessMap.setImgFilePath(BOSL::config::texturesDir + "\\fabric\\roughness.jpg");
    scene.useRoughnessMap = true;

    scene.material.sheen = 1.0f;
    //scene.material.sheenTint = 1.0f;

    return scene;
}

BOSL::Scene elephant()
{
    BOSL::Scene scene;

    scene.triangles = loadObj(BOSL::config::modelsDir + "\\elephant.obj");

    scene.cubemap.setDirName("SunriseCubemapBlurred");
    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 2.0f;

    scene.camera.setPosition(glm::vec3(-1.7f, 0.5f, 2.2f));

    scene.camera.setLookAt(glm::vec3(0.3f, 0.1f, 0.0f));

    scene.albedoMap.setImgFilePath(BOSL::config::texturesDir + "\\elephant\\albedo.jpg");
    scene.useAlbedoMap = true;
    scene.roughnessMap.setImgFilePath(BOSL::config::texturesDir + "\\elephant\\roughness.jpg");
    scene.useRoughnessMap = true;

    return scene;
}

BOSL::Scene suzanne()
{
    BOSL::Scene scene;

    scene.useCubemap = true;
    scene.hemisphereSamples = 4096;
    scene.exposure = 3.0f;
    
    scene.triangles = loadObj(BOSL::config::modelsDir + "\\suzanne.obj");

    scene.camera.setPosition(glm::vec3(-2.0f, 2.0f, 2.5f));

    scene.material.baseColor = glm::vec3(0.73f, 0.56f, 0.3f);
    scene.material.metallic = 1.0f;
    scene.material.roughness = 0.2f;

    return scene;
}