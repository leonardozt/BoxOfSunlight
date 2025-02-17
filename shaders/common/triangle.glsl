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
    // Tangent-Bitangent-Normal Matrix
    mat3 TBN;
};

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

    // Interpolate texture coordinates (or "UV coordinates")
    // (different meaning from barycentric coordinates)
    rec.uv = (1-u-v) * v0.uv + u * v1.uv + v * v2.uv;

    vec3 normal = texture(normalMap, rec.uv).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(triangle.TBN * normal);
    rec.normal = normal;  

    rec.hit = true;
    return rec;
}