#version 460
layout(local_size_x = 1, local_size_y = 1) in;

#include common\globals.glsl
#include common\input_output.glsl
#include common\hit.glsl
#include common\disney.glsl

// Stores the new, averaged color for the pixel at coordinates 'pixelCoords'
void storeColor(vec3 newColor, ivec2 pixelCoords) {
    // calculate average color
    vec3 prevColor = imageLoad(srcImage, pixelCoords).rgb;
    vec3 sum = prevColor * frameNumber;
    vec3 avgColor = (sum + newColor) / (frameNumber + 1);
    // store output color
    imageStore(dstImage, pixelCoords, vec4(avgColor, 1.0));
}

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

    // outgoing radiance
    vec3 Lo = vec3(0);
    if (info.hit) {
        // Solve the reflection equation
        
        vec3 V = normalize(camera.position - info.p); 
        
        vec3 N = info.N; // geometric normal
        vec3 T = info.T;
        vec3 B = info.B;

        if (useNormalMap)
        {
            vec3 texNormal = texture(normalMap, info.uv).rgb;
            texNormal = normalize(texNormal * 2.0 - 1.0);   
            N = normalize(mat3(T, B, N) * texNormal);   
        }
        Material newMaterial = material;
        if (useAlbedoMap) { newMaterial.baseColor = texture(albedoMap, info.uv).rgb; }
        if (useMetallicMap) { newMaterial.metallic = texture(metallicMap, info.uv).r; }
        if (useRoughnessMap) { newMaterial.roughness = texture(roughnessMap, info.uv).r; }

        if (!useCubemap) {
            // sample point light
            vec3 L = normalize(pLight.position - info.p);
            vec3 Li = pLight.emission;
            float NdotL = max(dot(N, L), 0.0);
            DisneyResults results = disneyBRDF(L, V, N, T, B, newMaterial);
            Lo = (results.diffuse + results.specular + results.clearCoat) * Li * NdotL;
        } else {
            // sample cubemap
            for (int s = 0; s < hemisphereSamples; s++)
            {    
                vec3 L = mat3(T,B,N) * uniformSampleHemisphere(randomFloat(), randomFloat());
                float NdotL = max(dot(N, L), 0.0);
                vec3 Li = texture(cubemap, L).rgb;
                DisneyResults results = disneyBRDF(L, V, N, T, B, newMaterial);                
                Lo += (results.diffuse + results.specular + results.clearCoat) * Li * NdotL;               
            }
            Lo *= (2*PI)/(hemisphereSamples);
        }
    } else {
        if (useCubemap) { Lo = texture(cubemap, ray.D).rgb; }
    }
    // Calculate new random state with xorshift and save
    rngStates[pixelCoords.y * imgRes.x + pixelCoords.x] = xorshift();
    // Store output color
    storeColor(Lo, pixelCoords);
}