#version 430
layout(local_size_x = 1, local_size_y = 1) in;

#define PI 3.14159265358979323

layout(rgba32f, binding = 0) uniform image2D outputImg;

uniform samplerCube cubemap;

uniform sampler2D wallNormalMap;
uniform sampler2D wallAlbedoMap;

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
    // texture coordinates
    vec2 uv;
    // distance along ray
    float t;
    //Material mat;
};

struct Vertex {
    // position
    vec3 pos;
    // UV coordinates
    vec2 uv;
};

struct Triangle {
    Vertex v0;
    Vertex v1;
    Vertex v2;
    //Material mat;

    // for testing
    mat3 TBN;
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

uniform Quad wall;

// for testing
struct PointLight {
    vec3 position;
    vec3 emission;
};
uniform PointLight pLight;

struct Sphere {
    vec3 center;
    float radius;
};
uniform Sphere sphere;

HitInfo sphereHit(Ray ray, Sphere sphere, Interval rayT);

void main() {
    // calculate dimensions of image
    ivec2 imgDims = imageSize(outputImg);
    // set base pixel color
    vec4 pixelColor = vec4(0.0, 0.0, 0.0, 1.0);
    // The Global Work Group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    // for testing -----------------------------------------
    Triangle triangles[2];
    triangles[0] = wall.t1;
    triangles[1] = wall.t2;    
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

    tempRec = sphereHit(ray, sphere, rayT);
    if (tempRec.hit) {
        hitAnything = true;
        rec = tempRec;
    }
    
    vec3 color;
    if (hitAnything) {
        // Blinn-Phong (for testing)

        vec3 albedo = texture(wallAlbedoMap, rec.uv).rgb;

        // ambient term
        vec3 ambientLight = texture(cubemap, rec.normal).rgb;
        vec3 ambient = ambientLight * albedo;

        vec3 toLight = normalize(pLight.position - rec.p);
        
        // diffuse term
        float normLightDot = max(0.0, dot(rec.normal, toLight));
        vec3 diffuse = pLight.emission * albedo * normLightDot;

        // specular term
        vec3 toV = normalize(camera.position - rec.p);
        vec3 h = normalize(toV + toLight);
        float specFactor = pow(max(0.0, dot(h, rec.normal)), 64.0);
        vec3 specular = vec3(0.6, 0.6, 0.6) * specFactor;

        color = ambient + diffuse + specular;
    } else {
        color = texture(cubemap, ray.dir).rgb;
    }
    pixelColor = vec4(color, 1.0);

    // store output pixel color
    imageStore(outputImg, pixelCoords, pixelColor);
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

    // Ignore triangle if it's back-facing (negative determinant)
    // or if it's parallel to the ray (determinant close to 0)
    if (det < 1e-8) {
        rec.hit = false;
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

    // Interpolate texture coordinates (or "UV coordinates")
    // (different meaning from barycentric coordinates)
    rec.uv = (1-u-v) * v0.uv + u * v1.uv + v * v2.uv;

    vec3 normal = texture(wallNormalMap, rec.uv).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(triangle.TBN * normal);
    rec.normal = normal;  

    rec.hit = true;
    return rec;
}

HitInfo sphereHit(Ray ray, Sphere sphere, Interval rayT) {
    HitInfo rec;
   
    vec3 oc = sphere.center - ray.origin;
    vec3 d = ray.dir;
    float radius = sphere.radius;

    float a = dot(d, d);
    float h = dot(d, oc);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = h * h - a * c;

    if (discriminant < 0) {
        rec.hit = false;
        return rec;
    }

    // Find the nearest root that lies in the acceptable range
    float sqrtd = sqrt(discriminant);
    float root = (h - sqrtd) / a;
    if (!((rayT.min <= root) && (root <= rayT.max))) {
        root = (h + sqrtd) / a;
        if (!((rayT.min <= root) && (root <= rayT.max))) {
            rec.hit = false;
            return rec;
        }
    }

    rec.t = root;
    rec.p = rayAt(ray, rec.t);
    
    // Calculate sphere UV from spherical coordinates
    vec3 normal = rec.p - sphere.center;
    float theta = acos(-normal.y);
    float phi = atan(-normal.z, normal.x) + PI; // atan is equivalent to atan2 in C (?)
    float u = phi / (2 * PI);
    float v = theta / PI;
    rec.uv = vec2(u, v);

    normal = texture(wallNormalMap, rec.uv).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(normal);
    vec3 tangent = vec3(sin(phi)*sin(theta), 0, -cos(phi)*sin(theta)); // check math
    tangent = normalize(tangent);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    rec.normal = normalize(TBN * normal);  

    rec.hit = true;
    return rec;
}
