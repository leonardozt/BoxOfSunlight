// Checks for ray-triangle hit using the Muller-Trumbore method
/*
HitInfo triangleHit(Ray r, Triangle triangle, Interval rayT)
{
    HitInfo rec;

    Vertex v0 = triangle.v0;
    Vertex v1 = triangle.v1;
    Vertex v2 = triangle.v2;
    vec3 e1 = vec3(v1.pos - v0.pos);
    vec3 e2 = vec3(v2.pos - v0.pos);

    vec3 p0 = vec3(v0.pos);

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

    rec.T = vec3(triangle.T);
    rec.B = vec3(triangle.B);
    rec.N = vec3(triangle.N);

    rec.hit = true;
    return rec;
}
*/

HitInfo triangleHit(Ray r, Triangle triangle, Interval rayT)
{
    HitInfo info;

    vec3 v0 = vec3(triangle.v0.pos);
    vec3 v1 = vec3(triangle.v1.pos);
    vec3 v2 = vec3(triangle.v2.pos);
    vec2 uv0 = triangle.v0.uv;
    vec2 uv1 = triangle.v1.uv;
    vec2 uv2 = triangle.v2.uv;

    vec3 T = vec3(triangle.T);
    vec3 B = vec3(triangle.B);
    vec3 N = vec3(triangle.N);
    
    // Ray-plane intersection
    float den = dot(N, r.dir);
    if (abs(den) < 1e-8) {
        info.hit = false;
        return info;
    }

    float d = -dot(N, v0);
    float t = -(dot(N, r.origin) + d) / den;

    if (!(rayT.min <= t && t <= rayT.max)) {
        info.hit = false;
        return info;
    }

    vec3 p = rayAt(r, t);

    // Pineda and Barycentric coordinates
    float area2 = length(cross(v1-v0, v2-v0));
    vec3 C;

    C = cross(v2-v1, p-v1);
    float u = length(C)/area2;
    float edge12 = dot(N, C);
    
    C = cross(v0-v2, p-v2);
    float v = length(C)/area2;
    float edge20 = dot(N, C);
    
    C = cross(v1-v0, p-v0);
    float edge01 = dot(N, C);

    if (!((edge01 >= 0 && edge12 >= 0 && edge20 >= 0)
            || (edge01 < 0 && edge12 < 0 && edge20 < 0))) {
        info.hit = false;
        return info;
    }

    info.hit = true;
    info.t = t;
    info.p = p;

    // Calculate UV coordinates (not the same as barycentric!)
    info.uv = u*uv0 + v*uv1 + (1.0f-u-v)*uv2;

    info.T = T;
    info.B = B;
    info.N = N;

    return info;
}

/*
HitInfo sphereHit(Ray ray, Sphere sphere, Interval rayT) {
    HitInfo rec;
   
    vec3 sphereCenter = sphere.center.xyz;
    vec3 oc = sphereCenter - ray.origin;
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
    vec3 sphereNormal = normalize(rec.p - sphereCenter);
    float theta = acos(-sphereNormal.y);
    float phi = atan(-sphereNormal.z, sphereNormal.x) + PI; // atan is equivalent to atan2 in C (?)
    float u = phi / (2 * PI);
    float v = theta / PI;
    rec.uv = vec2(u, v);

    vec3 tangent = vec3(sin(phi)*sin(theta), 0, cos(phi)*sin(theta)); // check math
    tangent = normalize(tangent);
    vec3 bitangent = cross(sphereNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, sphereNormal);

    // (test)
    rec.tangent = tangent;

    //vec3 normal = texture(normalMap, rec.uv).rgb;
    //normal = normal * 2.0 - 1.0;   
    //normal = normalize(normal);

    //rec.normal = normalize(TBN * normal);  
    rec.normal = normalize(rec.p - sphereCenter);

    rec.hit = true;
    return rec;
}
*/