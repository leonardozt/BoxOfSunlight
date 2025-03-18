#version 460
layout(local_size_x = 1, local_size_y = 1) in;

#include common\globals.glsl
#include common\input_output.glsl
#include common\hit.glsl
#include common\disney.glsl

void main() {
    // calculate image resolution
    ivec2 imgRes = imageSize(dstImage);
    // The Global Work Group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy) + ivec2(chunkOffset);

    // get random state
    rngState = rngStates[pixelCoords.y * imgRes.x + pixelCoords.x];

    Interval rayT = {0.1, 999};
    Ray ray = rayAroundPixel(pixelCoords, camera);

    HitInfo info = anyHit(ray, rayT);

    vec3 Lo = vec3(0);
    if (info.hit) {
        // Solve the reflection equation
        vec3 V = normalize(camera.position - info.p); 
        
        vec3 geometryNormal = info.N;
        vec3 T = info.T;
        vec3 B = info.B;
        /*
        vec3 texNormal = texture(normalMap, info.uv).rgb;
        texNormal = normalize(texNormal * 2.0 - 1.0);   
        vec3 N = normalize(mat3(T, B, geometryNormal) * texNormal);
        */
        vec3 N = geometryNormal;

        // (test)
        Material mat;
        //mat.baseColor = texture(albedoMap, info.uv).rgb;
        mat.baseColor = vec3(1.0f);
        mat.subsurface = 0;
        //mat.metallic = texture(metallicMap, info.uv).r;
        mat.metallic = 0.99;
        mat.specular = 1;
        mat.specularTint = 0;
        //mat.roughness = texture(roughnessMap, info.uv).r;
        mat.roughness = 0.5;
        mat.anisotropic = 0;
        mat.sheen = 0;
        mat.sheenTint = 0;
        mat.clearCoat = 0;
        mat.clearCoatGloss = 1;

        // sample point light
        vec3 L = normalize(pLight.position - info.p);
        vec3 Li = pLight.emission;
        float NdotL = max(dot(N, L), 0.0);
        DisneyResults results = disneyBRDF(L, V, N, T, B, mat);
        Lo = (results.diffuse + results.specular + results.clearCoat) * Li * NdotL;

        // sample cubemap
        /*
        uint hemisphereSamples = 10;
        for (int s = 0; s < hemisphereSamples; s++)
        {    
            L = uniformSampleHemisphere(randomFloat(), randomFloat());
            NdotL = max(dot(N, L), 0.0);
            Li = texture(cubemap, L).rgb;
            DisneyResults results = disneyBRDF(L, V, N, T, B, mat);                
            Lo += (results.diffuse + results.specular + results.clearCoat) * Li * NdotL;               
        }
        Lo *= (2*PI)/(hemisphereSamples);
        */
    } else {
        //Lo = texture(cubemap, ray.D).rgb;
    }

    // Calculate new random state with xorshift and save
    rngStates[pixelCoords.y * imgRes.x + pixelCoords.x] = xorshift();
    // Store output color
    storeColor(Lo, pixelCoords);
}