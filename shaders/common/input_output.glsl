layout(rgba32f) readonly uniform image2D srcImage;
layout(rgba32f) writeonly uniform image2D dstImage;

// Number of frames averaged into srcImage 
uniform uint frameNumber;

// Offset of current screen chunk
uniform uvec2 chunkOffset;

uniform samplerCube cubemap;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform bool useCubemap;

uniform bool useAlbedoMap;
uniform bool useNormalMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;

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

struct PointLight {
    vec3 position;
    vec3 emission;
};
uniform PointLight pLight;

struct Material {
    vec3 baseColor;
    float subsurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
};
uniform Material material;

uniform uint hemisphereSamples;