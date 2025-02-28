#define PI 3.14159265358979323
#define MAX_UINT 4294967295.0

struct Vertex {
    // position
    vec4 pos;
    // UV coordinates
    vec2 uv;
};

struct Triangle {
    Vertex v0;
    Vertex v1;
    Vertex v2;
    // Tangent-Bitangent-Normal Matrix
    mat4 TBN;
};

struct Sphere {
    vec4 center;
    float radius;
};

struct Ray {
    vec3 origin;
    // direction
    vec3 dir;
};

struct Viewport {
    // horizontal axis
    vec3 horiz;
    // vertical axis
    vec3 vert;
    // position of pixel (0,0) in 3D space
    vec3 pixel00;
    // horizontal distance between adjacent pixel centers
    vec3 deltaHoriz;
    // vertical distance between adjacent pixel centers
    vec3 deltaVert;
};

struct Camera {
    vec3 position;
    //float aperture;
    Viewport viewport;
};

struct Interval {
    float min;
    float max;
};

struct HitInfo {
    bool hit;
    // distance along ray
    float t;
    vec3 p;
    vec3 normal;
    // texture coordinates
    vec2 uv;
    
    // (test)
    vec3 tangent;
};

uint rngState;

uint xorshift() {
    rngState ^= (rngState << 13);
    rngState ^= (rngState >> 17);
    rngState ^= (rngState << 5);
    return rngState;
}

// Returns a random float in [0, 1]
float randomFloat() {
    return float(xorshift()) * (1.0 / MAX_UINT);    
}

vec2 sampleUnitSquare() {
    return vec2(randomFloat() - 0.5, randomFloat() - 0.5);
}

vec2 tentFilterPixel() {
    // 2x2 subpixel grid
    float r1 = 2 * randomFloat();
    float r2 = 2 * randomFloat();
    float dx;
    if (r1 < 1) {
        dx = sqrt(r1) - 1;
    } else {
        dx = 1 - sqrt(2 - r1);
    }
    float dy;
    if (r2 < 1) {
        dy = sqrt(r2) - 1;
    } else {
        dy = 1 - sqrt(2 - r2);
    }
    return vec2(r1, r2);
}

// Returns point along the ray at distance t from its origin
vec3 rayAt(Ray r, float t)
{
    return r.origin + r.dir * t;
}

// Returns ray passing through a random point point around
// the pixel coordinates pixelCoords
Ray rayAroundPixel(ivec2 pixelCoords, Camera camera)
{
    Viewport viewport = camera.viewport;
    vec2 offset = tentFilterPixel();
    vec3 randomPoint = viewport.pixel00
            + (float(pixelCoords.x)+offset.x) * viewport.deltaHoriz
            + (float(pixelCoords.y)+offset.y) * viewport.deltaVert;
    vec3 dir = normalize(randomPoint - camera.position);
    Ray r = {
            camera.position,
            dir
        };
    return r;
}