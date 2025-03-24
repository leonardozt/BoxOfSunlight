HitInfo triangleHit(Ray ray, Triangle triangle, Interval rayT)
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
    float den = dot(N, ray.D);
    if (abs(den) < 1e-8) {
        info.hit = false;
        return info;
    }

    float d = -dot(N, v0);
    float t = -(dot(N, ray.o) + d) / den;

    if (!contains(rayT, t)) {
        info.hit = false;
        return info;
    }

    vec3 p = rayAt(ray, t);

    // Pineda and Barycentric coordinates
    float area2 = length(cross(v1-v0, v2-v0));
    vec3 C;

    C = cross(v2-v1, p-v1);
    if (dot(N, C) < 0) {
        info.hit = false;
        return info;
    }
    float u = length(C)/area2;
    
    C = cross(v0-v2, p-v2);
    if (dot(N, C) < 0) {
        info.hit = false;
        return info;
    }
    float v = length(C)/area2;
    
    C = cross(v1-v0, p-v0);
    if (dot(N, C) < 0) {
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

HitInfo sphereHit(Ray ray, Sphere sphere, Interval rayT) {
    HitInfo info;
   
    vec3 sphereC = sphere.center.xyz;
    vec3 oc = sphereC - ray.o;
    float radius = sphere.radius;
    
    float a = dot(ray.D, ray.D);
    float h = dot(ray.D, oc);
    float c = dot(oc, oc) - radius*radius;
    float discriminant = h*h - a*c;

    if (discriminant < 0) {
        info.hit = false;
        return info;
    }

    // Find the nearest root that lies in the acceptable range
    float sqrtd = sqrt(discriminant);
    float root = (h - sqrtd) / a;
    if (!contains(rayT, root)) {
        root = (h + sqrtd) / a;
        if (!contains(rayT, root)) {
            info.hit = false;
            return info;
        }
    }

    info.hit = true;
    info.t = root;
    info.p = rayAt(ray, info.t);
    
    vec3 N = normalize(info.p - sphereC);

    // Calculate UV coordinates
    float theta = acos(N.y);
    float phi;
    if (abs(N.x) < 1e-8) {
        if (N.z < 0) {
            phi = PI / 2;  
        } else if (N.z > 0) {
            phi = -PI / 2; 
        } else {
            phi = 0.0; 
        }
    } else {
        phi = atan(-N.z, N.x); 
    }
    if (phi < 0) {
        // from [-PI, 0] to [PI, 2*PI]
        phi += 2*PI;
    }
    
    float u = 1 - (theta / PI);
    float v = phi / (2 * PI);
    info.uv = vec2(u, v);

    // Calculate tangent space coordinate frame
    vec3 T = -vec3(sin(theta)*sin(phi), 0, sin(theta)*cos(phi));
    T = normalize(T);
    vec3 B = cross(N, T);

    info.T = T;
    info.B = B;
    info.N = N;
    
    return info;
}

HitInfo anyHit(Ray ray, Interval rayT)
{        
    HitInfo info;
    info.hit = false;

    HitInfo tempInfo;    
    float closestSoFar = rayT.max;
    
    // check intersections with spheres
    for (int i = 0; i < spheres.length(); i++) {
        tempInfo = sphereHit(ray, spheres[i], rayT);
        if (tempInfo.hit) {
            closestSoFar = tempInfo.t;
            info = tempInfo;
            rayT.max = closestSoFar;
        }
    }

    // check intersections with triangles
    for (int i = 0; i < triangles.length(); i++) {
        tempInfo = triangleHit(ray, triangles[i], rayT);
        if (tempInfo.hit) {
            closestSoFar = tempInfo.t;
            info = tempInfo;
            rayT.max = closestSoFar;
        }
    }
    return info;
}