#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D outputImg;

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
  vec3 e1; // v1 - v0
  vec3 e2; // v2 - v0
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

void main() {
  // -----------------------------------------------------
  // calculate dimensions of image
  ivec2 imgDims = imageSize(outputImg);
  // set base pixel color
  vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
  // The Global Work Group ID contains the coordinates
  // of the pixel that we're rendering 
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
  // -----------------------------------------------------
  // -----------------------------------------------------
  vec3 normal = vec3(0.0, 0.0, 1.0);
  vec3 pos0 = vec3(-1.0, -1.0, -1.0);
  vec3 pos1 = vec3( 1.0, -1.0, -1.0);
  vec3 pos2 = vec3( 0.0,  1.0, -1.0);  
  Vertex v0 = {pos0, normal};
  Vertex v1 = {pos1, normal};
  Vertex v2 = {pos2, normal};
  Triangle triangle = {v0, v1, v2, pos1-pos0, pos2-pos0};
  // -----------------------------------------------------
  // -----------------------------------------------------
  Interval rayT = {0.1, 999};
  Ray r = rayThroughPixel(pixelCoords);
  HitInfo rec = triangleHit(r, triangle, rayT); 
  if (rec.hit) {
    pixel = vec4(1.0, 0.0, 0.0, 1.0);  
  } else {
    float a = 0.5*(normalize(r.dir).y + 1.0);
    pixel = vec4( (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0), 1.0);
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
                     + float(pixelCoords.x)*viewport.deltaHoriz
                     + float(pixelCoords.y)*viewport.deltaVert;
  vec3 dir = normalize(pixelCenter - camera.position); 
  Ray r = {camera.position, dir};
  return r;
}

// Checks for ray-triangle hit using the Muller-Trumbore method
HitInfo triangleHit(Ray r, Triangle triangle, Interval rayT)
{
    HitInfo rec;

    Vertex v0 = triangle.v0;
    Vertex v1 = triangle.v1;
    Vertex v2 = triangle.v2;
    vec3 e1 = triangle.e1;
    vec3 e2 = triangle.e2;

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
    vec3 normal = v0.normal*(1-u-v) + v1.normal*u + v2.normal*v; 
    
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