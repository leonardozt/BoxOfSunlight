#version 430
layout(local_size_x = 1, local_size_y = 1) in;

#include common\constants.glsl
#include common\uniforms.glsl
#include common\globals.glsl
#include common\triangle.glsl
#include common\sphere.glsl

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

// for testing
uniform Sphere sphere;

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

        vec3 albedo = texture(albedoMap, rec.uv).rgb;

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
        float specFactor = pow(max(0.0, dot(h, rec.normal)), 80.0);
        vec3 specular = vec3(0.6, 0.6, 0.6) * specFactor;

        //color = ambient + diffuse + specular;
        color = diffuse;
    } else {
        color = texture(cubemap, ray.dir).rgb;
    }
    pixelColor = vec4(color, 1.0);

    // store output pixel color
    imageStore(outputImg, pixelCoords, pixelColor);
}