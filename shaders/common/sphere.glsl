struct Sphere {
    vec3 center;
    float radius;
};

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
    vec3 sphereNormal = normalize(rec.p - sphere.center);
    float theta = acos(-sphereNormal.y);
    float phi = atan(-sphereNormal.z, sphereNormal.x) + PI; // atan is equivalent to atan2 in C (?)
    float u = phi / (2 * PI);
    float v = theta / PI;
    rec.uv = vec2(u, v);

    vec3 tangent = vec3(sin(phi)*sin(theta), 0, cos(phi)*sin(theta)); // check math
    tangent = normalize(tangent);
    vec3 bitangent = cross(sphereNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, sphereNormal);

    vec3 normal = texture(normalMap, rec.uv).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(normal);

    rec.normal = normalize(TBN * normal);  

    rec.hit = true;
    return rec;
}