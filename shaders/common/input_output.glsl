layout(rgba32f, binding = 0) uniform image2D outputImg;

uniform samplerCube cubemap;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform Camera camera;

layout(std430, binding = 1) readonly buffer trianglesBuf {
    Triangle triangles[];
};

layout(std430, binding = 2) readonly buffer spheresBuf {
    Sphere spheres[];
};

// for testing
struct PointLight {
    vec3 position;
    vec3 emission;
};
uniform PointLight pLight;