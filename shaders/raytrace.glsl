#version 460
layout(local_size_x = 1, local_size_y = 1) in;

#include common\globals.glsl
#include common\input_output.glsl
#include common\hit.glsl

/*
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
*/
/*
vec3 schlickFresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
*/
/*
vec3 cookTorranceBRDF(vec2 uv, vec3 V, vec3 N, vec3 L)
{
        vec3 albedo = texture(albedoMap, uv).rgb;
        float metallic = texture(metallicMap, uv).r;
        float roughness = texture(roughnessMap, uv).r;
        
        // Fresnel
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);
        
        // radiance
        vec3 H = normalize(V + L);
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = schlickFresnel(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic; // nullify kD if surface is metallic
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
        
        return kD * albedo / PI + specular;
}
*/

// Stores the new, averaged color for the pixel at coordinates 'pixelCoords'
void storeColor(vec3 newColor, ivec2 pixelCoords) {
    // calculate average color
    vec3 prevColor = imageLoad(srcImage, pixelCoords).rgb;
    vec3 sum = prevColor * frameNumber;
    vec3 avgColor = (sum + newColor) / (frameNumber + 1);
    // store output color
    imageStore(dstImage, pixelCoords, vec4(avgColor, 1.0));
}

vec3 cosineWeightedSampleOnHemisphere(float u1, float u2) {
    float cosTheta = u1;
    float sinTheta = sqrt(1.0 - pow(u1,2));
    float phi = 2.0 * PI * u2;
    return vec3(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
}

#include common\disney.glsl

void main() {
    // calculate image resolution
    ivec2 imgRes = imageSize(dstImage);
    // set base pixel color
    vec4 pixelColor = vec4(0.0, 0.0, 0.0, 1.0);
    // The Global Work Group ID contains the coordinates
    // of the pixel that we're rendering
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy) + ivec2(chunkOffset);

    rngState = rngStates[pixelCoords.y * imgRes.x + pixelCoords.x];

    Interval rayT = {0.1, 999};
    Ray ray = rayAroundPixel(pixelCoords, camera);
    
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
        /*
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
        float specFactor = pow(max(0.0, dot(h, info.normal)), 8.0);
        vec3 specular = vec3(0.6, 0.6, 0.6) * specFactor;

        //color = ambient + diffuse + specular;
        color = ambient + specular;
        */

        // Solve the rendering equation
        vec3 V = normalize(camera.position - info.p); 
        
        vec3 geometricNormal = info.normal;
        vec3 tangent = info.tangent;
        vec3 bitangent = normalize(cross(geometricNormal, tangent));
        mat3 TBN = mat3(tangent, bitangent, geometricNormal);
        vec3 texNormal = texture(normalMap, info.uv).rgb;
        vec3 N = normalize(TBN * texNormal);

        // (test)
        DisneyMaterial mat;
        //mat.baseColor = texture(albedoMap, info.uv).rgb;
        mat.baseColor = vec3(1.0f);
        mat.subsurface = 0;
        //mat.metallic = texture(metallicMap, info.uv).r;
        mat.metallic = 0.99;
        mat.specular = 1;
        mat.specularTint = 0;
        mat.roughness = texture(roughnessMap, info.uv).r;
        mat.anisotropic = 0.8;
        mat.sheen = 0;
        mat.sheenTint = 0.5;
        mat.clearCoat = 0;
        mat.clearCoatGloss = 1;

        // sample direct light
        vec3 L = normalize(pLight.position - info.p);
        vec3 Li = pLight.emission;
        float NdotL = max(dot(N, L), 0.0);

        // (test)
        BRDFResults results = disneyBRDF(L, V, N, info.tangent, bitangent, mat);
        vec3 Lo = (results.diffuse + results.specular + results.clearCoat) * Li * NdotL;
        
        // sample cubemap
        /*
        vec3 w = N;
        if (dot(N, ray.dir)>=0) w *= -1;
        vec3 tmp;
        if (w.x > 0.1) {
            tmp = vec3(0.0, 1.0, 0.0);
        } else {
            tmp = vec3(1.0, 0.0, 0.0);
        }
        vec3 u = normalize(cross(tmp, w));
        vec3 v = cross(w, u);

        int numSamples = 0;
        for (int i = 0; i < 100; i++) {
            L = cosineWeightedSampleOnHemisphere(randomFloat(), randomFloat());
            L = normalize(vec3(L.x * u + L.y * v + L.z * w));
            vec3 Li = texture(cubemap, L).rgb;                
            vec3 radiance = cookTorranceBRDF(info.uv, V, N, L) * Li * PI;
            Lo = (Lo*numSamples + radiance)/(numSamples+1);
            numSamples++;
        }
        */
        color = Lo;
    } else {
        color = texture(cubemap, ray.dir).rgb;
    }

    // Calculate new random state with xorshift and save
    rngStates[pixelCoords.y * imgRes.x + pixelCoords.x] = xorshift();
    // Store output color
    storeColor(color, pixelCoords);
}