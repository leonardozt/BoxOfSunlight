#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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
    // The work group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_WorkGroupID.xy) + ivec2(chunkOffset);

    // get random state
    rngState = rngStates[pixelCoords.y * imgRes.x + pixelCoords.x];

    Interval rayT = {0.1, 999};
    Ray ray = rayAroundPixel(pixelCoords, camera);

    HitInfo info = anyHit(ray, rayT);
    
    vec3 Lo = vec3(0); // outgoing radiance
    if (info.hit) {
        // Solve the reflection equation
        
        vec3 V = normalize(camera.position - info.p); 
        
        vec3 geometricNormal = info.N;
        vec3 T = info.T;
        vec3 B = info.B;
        vec3 N = geometricNormal;

        if (useNormalMap)
        {
            vec3 texNormal = texture(normalMap, info.uv).rgb;
            texNormal = normalize(texNormal * 2.0 - 1.0);   
            N = normalize(mat3(T, B, geometricNormal) * texNormal);               
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
            vec3 brdfValue = results.diffuse + results.specular + results.clearCoat;
            Lo = brdfValue * Li * NdotL;

        } else {
            // sample cubemap
            for (int s = 0; s < hemisphereSamples; s++)
            {
                float u1 = randomFloat(); float u2 = randomFloat();
                vec3 L = mat3(T,B,geometricNormal) * uniformSampleHemisphere(u1, u2);
                float NdotL = max(dot(N, L), 0.0);
                vec3 Li = texture(cubemap, L).rgb;
                
                DisneyResults results = disneyBRDF(L, V, N, T, B, newMaterial); 
                vec3 brdfValue = results.diffuse + results.specular + results.clearCoat;                

                Lo += brdfValue * Li * NdotL;               
            }
            Lo *= (2*PI)/(hemisphereSamples);
        }
    } else {
        if (useCubemap) { Lo = texture(cubemap, ray.D).rgb; }
    }
    // Calculate and store new random state with xorshift
    rngStates[pixelCoords.y * imgRes.x + pixelCoords.x] = xorshift();
    // Store output color
    storeColor(Lo, pixelCoords);
}