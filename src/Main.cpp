#include "utils\Config.h"
#include "utils\BoxOfSunlightError.h"
#include "utils\Debug.h"
#include "rendering\Window.h"
#include "rendering\Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader\tiny_obj_loader.h>

BOSL::Scene createSpheres();
BOSL::Scene loadObj(std::string objPath);

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
        
        BOSL::Scene scene = loadObj(BOSL::config::modelsDir + "cube.obj");
        scene.camera.setPosition(glm::vec3(0.0f, 2.0f, 10.0f));
        
        //BOSL::Scene scene = createSpheres();
        //scene.camera.setPosition(glm::vec3(10.0f, 10.0f, 20.0f));
        
        scene.albedoMap.setImgFilePath(BOSL::config::imagesDir+"textures\\rock_face_03_diff_4k.jpg");
        scene.normalMap.setImgFilePath(BOSL::config::imagesDir + "textures\\rock_face_03_nor_gl_4k.jpg");
        scene.metallicMap.setImgFilePath(BOSL::config::imagesDir + "textures\\rock_face_03_metal_4k.jpg");
        scene.roughnessMap.setImgFilePath(BOSL::config::imagesDir + "textures\\rock_face_03_rough_4k.jpg");

        // Point light (for testing)
        BOSL::PointLight pLight;
        pLight.position = glm::vec3(5.0f, 3.0f, 8.0f);
        pLight.emission = glm::vec3(1.0f);
        scene.pLight = pLight;

        // Create renderer object
        BOSL::Renderer renderer(std::move(scene));

        // ------------------------------------------------------------------
        // timing 
        double deltaTime = 0.0f; // time between current frame and last frame
        double lastFrame = 0.0f; // time of last frame
        int fCounter = 0;
        // ------------------------------------------------------------------

        while (!window.shouldClose())
        {
            renderer.render();
            window.update();
            glfwPollEvents();

            // --------------------------------------------------------------
            // Set frame time
            double currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            // Print only every 100 frames
            if (fCounter > 100) {
                std::cout << "FPS: " << 1 / deltaTime << std::endl;
                fCounter = 0;
            }
            else {
                fCounter++;
            }
            // --------------------------------------------------------------
        }        

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        exitStatus = EXIT_FAILURE;
    }

    glfwTerminate();
    return exitStatus;
}

BOSL::Scene createSpheres() {
    BOSL::Scene scene;

    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 3.0f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-5.0f, 1.0f, -5.0f, 1.0f), 1.0f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(4.0f, -1.0f, -1.5f, 1.0f), 1.0f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-1.5f, -1.5f, 20.0f, 1.0f), 0.5f });
    //scene.spheres.push_back(BOSL::Sphere{ glm::vec4(1.8f, 2.3f, -2.0f, 1.0f), 2.0f });

    return scene;
}

BOSL::Scene loadObj(std::string objPath) {
    BOSL::Scene scene;

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

    std::vector<BOSL::Triangle> triangles;

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
            BOSL::Triangle t;
            t.v0 = vertices[0];
            t.v1 = vertices[1];
            t.v2 = vertices[2];
            BOSL::calculateTBN(t);
            triangles.push_back(t);

            index_offset += 3;
        }
    }
    scene.triangles = triangles;
    return scene;
}