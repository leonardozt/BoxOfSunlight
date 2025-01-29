#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D outputImg;

uniform samplerCube cubemap;

struct Ray {
    vec3 origin;
    // direction
    vec3 dir;
};
vec3 rayAt(Ray r, float t);
Ray rayThroughPixel(ivec2 pixelCoords);

struct Interval {
    float min;
    float max;
};

struct HitInfo {
    bool hit;
    vec3 p;
    vec3 normal;
    // distance along ray
    float t;
    bool frontFace;
    //Material mat;
};

struct Vertex {
    // position
    vec3 pos;
    vec3 normal;
    //vec2 texCoord;
};

struct Triangle {
    Vertex v0;
    Vertex v1;
    Vertex v2;
    //Material mat;
};
HitInfo triangleHit(Ray r, Triangle triangle, Interval rayT);

struct Camera {
    vec3 position;
    //float aperture;
};
uniform Camera camera;

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
uniform Viewport viewport;

// for testing
struct Quad {
    Triangle t1;
    Triangle t2;
};
// for testing
struct Cube {
    Quad front;
    Quad right;
    Quad back;
    Quad left;
    Quad top;
    Quad bottom;
};

Cube testCube()
{
    vec3 p0 = vec3(-1.0, -1.0,  1.0);
    vec3 p1 = vec3( 1.0, -1.0,  1.0);
    vec3 p2 = vec3( 1.0,  1.0,  1.0);
    vec3 p3 = vec3(-1.0,  1.0,  1.0);
    vec3 p4 = vec3(-1.0, -1.0, -1.0);
    vec3 p5 = vec3( 1.0, -1.0, -1.0);
    vec3 p6 = vec3( 1.0,  1.0, -1.0);    
    vec3 p7 = vec3(-1.0,  1.0, -1.0);
    
    Quad faces[6];
    Triangle t1;
    Triangle t2;

    // front face
    vec3 normal = vec3(0.0, 0.0, 1.0);
    t1.v0.pos = p0;
    t1.v0.normal = normal;
    t1.v1.pos = p1;
    t1.v1.normal = normal;
    t1.v2.pos = p3;
    t1.v2.normal = normal;
    t2.v0.pos = p1;
    t2.v0.normal = normal;
    t2.v1.pos = p2;
    t2.v1.normal = normal;
    t2.v2.pos = p3;
    t2.v2.normal = normal;
    faces[0].t1 = t1;
    faces[0].t2 = t2;
    // right
    normal = vec3(1.0, 0.0, 0.0);
    t1.v0.pos = p1;
    t1.v0.normal = normal;
    t1.v1.pos = p5;
    t1.v1.normal = normal;
    t1.v2.pos = p2;
    t1.v2.normal = normal;
    t2.v0.pos = p5;
    t2.v0.normal = normal;
    t2.v1.pos = p6;
    t2.v1.normal = normal;
    t2.v2.pos = p2;
    t2.v2.normal = normal;
    faces[1].t1 = t1;
    faces[1].t2 = t2;
    // back
    normal = vec3(0.0, 0.0, -1.0);
    t1.v0.pos = p4;
    t1.v0.normal = normal;
    t1.v1.pos = p5;
    t1.v1.normal = normal;
    t1.v2.pos = p7;
    t1.v2.normal = normal;
    t2.v0.pos = p5;
    t2.v0.normal = normal;
    t2.v1.pos = p6;
    t2.v1.normal = normal;
    t2.v2.pos = p7;
    t2.v2.normal = normal;
    faces[2].t1 = t1;
    faces[2].t2 = t2;
    // left
    normal = vec3(-1.0, 0.0, 0.0);
    t1.v0.pos = p4;
    t1.v0.normal = normal;
    t1.v1.pos = p0;
    t1.v1.normal = normal;
    t1.v2.pos = p7;
    t1.v2.normal = normal;
    t2.v0.pos = p0;
    t2.v0.normal = normal;
    t2.v1.pos = p3;
    t2.v1.normal = normal;
    t2.v2.pos = p7;
    t2.v2.normal = normal;
    faces[3].t1 = t1;
    faces[3].t2 = t2;
    // top
    normal = vec3(0.0, 1.0, 0.0);
    t1.v0.pos = p3;
    t1.v0.normal = normal;
    t1.v1.pos = p2;
    t1.v1.normal = normal;
    t1.v2.pos = p7;
    t1.v2.normal = normal;
    t2.v0.pos = p2;
    t2.v0.normal = normal;
    t2.v1.pos = p6;
    t2.v1.normal = normal;
    t2.v2.pos = p7;
    t2.v2.normal = normal;
    faces[4].t1 = t1;
    faces[4].t2 = t2;
    // bottom
    normal = vec3(0.0, -1.0, 0.0);
    t1.v0.pos = p4;
    t1.v0.normal = normal;
    t1.v1.pos = p5;
    t1.v1.normal = normal;
    t1.v2.pos = p0;
    t1.v2.normal = normal;
    t2.v0.pos = p5;
    t2.v0.normal = normal;
    t2.v1.pos = p1;
    t2.v1.normal = normal;
    t2.v2.pos = p0;
    t2.v2.normal = normal;
    faces[5].t1 = t1;
    faces[5].t2 = t2;
    
    Cube cube;
    cube.front = faces[0];
    cube.right = faces[1];
    cube.back = faces[2];
    cube.left = faces[3];
    cube.top = faces[4];
    cube.bottom = faces[5];

    return cube;
}

void main() {
    // calculate dimensions of image
    ivec2 imgDims = imageSize(outputImg);
    // set base pixel color
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
    // The Global Work Group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    // -----------------------------------------------------
    // unpack cube (for testing)
    Cube cube = testCube();
    Triangle triangles[12];
    triangles[0] = cube.front.t1;
    triangles[1] = cube.front.t2;
    triangles[2] = cube.right.t1;
    triangles[3] = cube.right.t2;
    triangles[4] = cube.back.t1;
    triangles[5] = cube.back.t2;
    triangles[6] = cube.left.t1;
    triangles[7] = cube.left.t2;
    triangles[8] = cube.top.t1;
    triangles[9] = cube.top.t2;
    triangles[10] = cube.bottom.t1;
    triangles[11] = cube.bottom.t2;
    // -----------------------------------------------------
    
    Interval rayT = {0.1, 999};
    Ray ray = rayThroughPixel(pixelCoords);
    
    HitInfo rec;
    HitInfo tempRec;
    bool hitAnything = false;
    float closestSoFar = rayT.max;
    for (int i = 0; i < triangles.length(); i++) {
        tempRec = triangleHit(ray, triangles[i], rayT);
        if (tempRec.hit) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
            rayT.max = closestSoFar;
        }
    }

    if (hitAnything) {
        // mirror (for testing)
        vec3 reflectedRay = reflect(ray.dir, rec.normal);
        pixel = texture(cubemap, reflectedRay);
    } else {
        pixel = texture(cubemap, ray.dir);
    }

    // store output pixel color
    imageStore(outputImg, pixelCoords, pixel);
}

// Returns point along the ray at distance t from its origin
vec3 rayAt(Ray r, float t)
{
    return r.origin + r.dir * t;
}

// Returns ray passing through the center of pixel
// with coordinates pixelCoords
Ray rayThroughPixel(ivec2 pixelCoords)
{
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

// Checks for ray-triangle hit using the Muller-Trumbore method
HitInfo triangleHit(Ray r, Triangle triangle, Interval rayT)
{
    HitInfo rec;

    Vertex v0 = triangle.v0;
    Vertex v1 = triangle.v1;
    Vertex v2 = triangle.v2;
    vec3 e1 = v1.pos - v0.pos;
    vec3 e2 = v2.pos - v0.pos;

    vec3 p0 = v0.pos;

    vec3 pvec = cross(r.dir, e2);
    float det = dot(pvec, e1);

    if (abs(det) < 1e-8) {
        rec.hit = false; // triangle and ray are parallel
        return rec;
    }

    float invDet = 1.0 / det;

    vec3 tvec = r.origin - p0;
    float u = dot(pvec, tvec) * invDet;
    if (u < 0.0 || u > 1.0) {
        rec.hit = false;
        return rec;
    }

    vec3 qvec = cross(tvec, e1);
    float v = dot(qvec, r.dir) * invDet;
    if (v < 0.0 || u + v > 1.0) {
        rec.hit = false;
        return rec;
    }

    float t = dot(qvec, e2) * invDet;
    if (!(rayT.min <= t && t <= rayT.max)) {
        rec.hit = false;
        return rec;
    }

    vec3 intersection = rayAt(r, t);

    rec.t = t;
    rec.p = intersection;
    //rec.material = triangle.mat;

    // Interpolate normal values from vertices
    vec3 normal = v0.normal * (1 - u - v) + v1.normal * u + v2.normal * v;

    bool frontFace = (dot(r.dir, normal) < 0);
    rec.frontFace = frontFace;
    rec.normal = frontFace ? normal : -normal;

    // Interpolate texture coordinates (u and v)
    // (different meaning from barycentric coordinates)
    //rec.u = v0.texCoords.x*(1-u-v) + v1.texCoords.x*u + v2.texCoords.x*v;
    //rec.v = v0.texCoords.y*(1-u-v) + v1.texCoords.y*u + v2.texCoords.y*v;

    rec.hit = true;
    return rec;
}
