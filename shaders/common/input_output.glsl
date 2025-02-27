layout(rgba32f) readonly uniform image2D srcImage;
layout(rgba32f) writeonly uniform image2D dstImage;

// Number of frames averaged into srcImage 
uniform uint frameNumber;

uniform samplerCube cubemap;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform Camera camera;

layout(std430, binding = 0) readonly buffer trianglesBuf {
    Triangle triangles[];
};

layout(std430, binding = 1) readonly buffer spheresBuf {
    Sphere spheres[];
};

layout(std430, binding = 2) buffer rngStateBuf {
    uint rngStates[];
};

// for testing
struct PointLight {
    vec3 position;
    vec3 emission;
};
uniform PointLight pLight;

// Offset of current chunk (test)
uniform uvec2 chunkOffset;