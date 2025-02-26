#version 430
layout(local_size_x = 1, local_size_y = 1) in;

#include common\constants.glsl
#include common\globals.glsl
#include common\input_output.glsl
#include common\hit.glsl

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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

        vec3 N = normalize(info.normal);
        vec3 V = normalize(info.p);

        vec3 albedo = texture(albedoMap, info.uv).rgb;
        
        float metallic = texture(metallicMap, info.uv).r;
        float roughness = texture(roughnessMap, info.uv).r;
        
        // Fresnel
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // Rendering equation (with single point light)
        vec3 Lo = vec3(0.0);
        // radiance
        vec3 L = normalize(pLight.position - info.p);
        vec3 H = normalize(V + L);
        vec3 radiance = pLight.emission;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic; // nullify kD if surface is metallic
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        float NdotL = max(dot(N, L), 0.0);                
        Lo = (kD * albedo / PI + specular) * radiance * NdotL; 

        color = Lo;
    } else {
        color = texture(cubemap, ray.dir).rgb;
    }
    
    pixelColor = vec4(color, 1.0);

    // store output pixel color
    imageStore(outputImg, pixelCoords, pixelColor);
}