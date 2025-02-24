#include "utils\Config.h"
#include "utils\BoxOfSunlightError.h"
#include "utils\Debug.h"
#include "rendering\Window.h"
#include "rendering\Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader\tiny_obj_loader.h>

// Callback function for GLFW errors
void glfwErrorCallback(int error, const char* description)
{
    std::string mess = "GLFW Error: " + std::string(description) + "\n";
    throw BoxOfSunlightError(mess);
}

// Initializes GLEW and sets the needed OpenGL settings
void initGL()
{
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        throw BoxOfSunlightError("GLEW failed to initialize");
    }
    // Enable Debug Mode
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(BOSL::debug::debugMessageCallback, 0);
    // Set Viewport
    glViewport(0, 0, BOSL::config::windowWidth, BOSL::config::windowHeight);
    // Print limitations for Compute Shaders
    BOSL::debug::printComputeLimits();
}

BOSL::Scene createSpheres() {
    BOSL::Scene scene;

    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 3.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-5.0f, 1.0f, -5.0f, 1.0f), 1.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(4.0f, -1.0f, -1.5f, 1.0f), 1.0f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(-1.5f, -1.5f, 20.0f, 1.0f), 0.5f });
    scene.spheres.push_back(BOSL::Sphere{ glm::vec4(1.8f, 2.3f, -2.0f, 1.0f), 2.0f });

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
            glm::mat4 TBN = BOSL::triangleTBN(vertices[0],vertices[1],vertices[2]);
            triangles.push_back(BOSL::Triangle{ vertices[0],vertices[1],vertices[2],TBN });
            
            index_offset += 3;
        }
    }
    scene.triangles = triangles;
    return scene;
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);

    int exitStatus = EXIT_SUCCESS;

    try {    
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        Window window(
            BOSL::config::windowWidth,
            BOSL::config::windowHeight,
            BOSL::config::windowTitle
        );
    
        initGL();
        
        // Set up scene
        /*
        BOSL::Scene scene = loadObj(BOSL::config::modelsDir + "cube.obj");
        scene.albedoMapImgPath = BOSL::config::imagesDir + "Texturelabs_Stone_124S.jpg";
        scene.normalMapImgPath = BOSL::config::imagesDir + "testNormalMap.jpg";
        scene.camera.setPosition(glm::vec3(-4.0f, 1.3f, 5.5f));
        scene.camera.setLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
        */
        BOSL::Scene scene = createSpheres();
        scene.albedoMapImgPath = BOSL::config::imagesDir + "uniformBlue.jpg";
        scene.normalMapImgPath = BOSL::config::imagesDir + "uniformNormalMap.jpg";
        scene.camera.setPosition(glm::vec3(0.0f, 0.0f, 30.0f));
        // Point light (for testing)
        BOSL::PointLight pLight;
        pLight.position = glm::vec3(5.0f, 2.5f, 1.5f);
        pLight.emission = glm::vec3(5.0f, 5.0f, 5.1f);
        scene.pLight = pLight;

        // Create renderer object
        BOSL::Renderer renderer(std::move(scene));
        
        // timing 
        double deltaTime = 0.0f; // time between current frame and last frame
        double lastFrame = 0.0f; // time of last frame
        int fCounter = 0;

        while (!window.shouldClose())
        {
            renderer.render();
            window.update();
            glfwPollEvents();

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

    } catch(BoxOfSunlightError& err) {
        std::cerr << "BoxOfSunlightError - " << err.what() << std::endl;
        exitStatus = EXIT_FAILURE;
    }

    glfwTerminate();
    return exitStatus;
}