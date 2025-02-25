#version 430
layout(local_size_x = 1, local_size_y = 1) in;

#include common\constants.glsl
#include common\globals.glsl
#include common\input_output.glsl
#include common\hit.glsl

void main() {
    // calculate dimensions of image
    ivec2 imgDims = imageSize(outputImg);
    // set base pixel color
    vec4 pixelColor = vec4(0.0, 0.0, 0.0, 1.0);
    // The Global Work Group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    Interval rayT = {0.1, 999};
    Ray ray = rayThroughPixel(pixelCoords, camera);
    
    HitInfo info;
    HitInfo tempInfo;
    bool hitAnything = false;
    float closestSoFar = rayT.max;
    
    // Check intersections with triangles
    for (int i = 0; i < triangles.length(); i++) {
        tempInfo = triangleHit(ray, triangles[i], rayT);
        if (tempInfo.hit) {
            hitAnything = true;
            closestSoFar = tempInfo.t;
            info = tempInfo;
            rayT.max = closestSoFar;
        }
    }
    // Check intersections with spheres
    for (int i = 0; i < spheres.length(); i++) {
        tempInfo = sphereHit(ray, spheres[i], rayT);
        if (tempInfo.hit) {
            hitAnything = true;
            closestSoFar = tempInfo.t;
            info = tempInfo;
            rayT.max = closestSoFar;
        }
    }

    vec3 color = vec3(0);
    if (hitAnything) {
        // Blinn-Phong (for testing)

        vec3 albedo = texture(albedoMap, info.uv).rgb;

        // ambient term
        vec3 ambientLight = texture(cubemap, info.normal).rgb;
        vec3 ambient = ambientLight * albedo;

        vec3 toLight = normalize(pLight.position - info.p);
        
        // diffuse term
        float normLightDot = max(0.0, dot(info.normal, toLight));
        vec3 diffuse = pLight.emission * albedo * normLightDot;

        // specular term
        vec3 toV = normalize(camera.position - info.p);
        vec3 h = normalize(toV + toLight);
        float specFactor = pow(max(0.0, dot(h, info.normal)), 80.0);
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