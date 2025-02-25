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
};

// Returns point along the ray at distance t from its origin
vec3 rayAt(Ray r, float t)
{
    return r.origin + r.dir * t;
}

// Returns ray passing through the center of pixel
// with coordinates pixelCoords
Ray rayThroughPixel(ivec2 pixelCoords, Camera camera)
{
    Viewport viewport = camera.viewport;
    vec3 pixelCenter = viewport.pixel00
            + float(pixelCoords.x) * viewport.deltaHoriz
            + float(pixelCoords.y) * viewport.deltaVert;
    vec3 dir = normalize(pixelCenter - camera.position);
    Ray r = {
            camera.position,
            dir
        };
    return r;
}
