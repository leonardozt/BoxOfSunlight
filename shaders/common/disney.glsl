float sqr(float x) { return x*x; }

float schlickFresnel(float u)
{
    float m = clamp(1-u, 0, 1);
    float m2 = m*m;
    return m2*m2*m; // pow(m,5)
}

float GTR1(float NdotH, float a)
{
    //if (a >= 1) return 1/PI;
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return (a2-1) / (PI*log(a2)*t);
}

float GTR2(float NdotH, float a)
{
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return a2 / (PI * t*t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
    return 1 / (PI * ax*ay * sqr( sqr(HdotX/ax) + sqr(HdotY/ay) + NdotH*NdotH ));
}

float smithG_GGX(float NdotV, float alphaG)
{
    float a = alphaG*alphaG;
    float b = NdotV*NdotV;
    return 1 / (NdotV + sqrt(a + b - a*b));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay) {
    return 1 / (NdotV + sqrt(sqr(VdotX * ax) + sqr(VdotY * ay) + sqr(NdotV)));
}

struct DisneyResults {
    vec3 diffuse;
    vec3 specular;
    vec3 clearCoat;
};

DisneyResults disneyBRDF(vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, Material mat)
{
    DisneyResults results = {vec3(0.0f), vec3(0.0f), vec3(0.0f)};

    vec3 baseColor = mat.baseColor;
    float subsurface = mat.subsurface;
    float metallic = mat.metallic;
    float specular = mat.specular;
    float specularTint = mat.specularTint;
    float roughness = mat.roughness;
    float anisotropic = mat.anisotropic;
    float sheen = mat.sheen;
    float sheenTint = mat.sheenTint;
    float clearCoat = mat.clearCoat;
    float clearCoatGloss = mat.clearCoatGloss;

    float NdotL = dot(N,L);
    float NdotV = dot(N,V);

    if (NdotL < 0 || NdotV < 0) {
        //results.diffuse = vec3(0.01f);
        return results;
    }

    vec3 H = normalize(L+V);
    float NdotH = dot(N,H);
    float LdotH = dot(L,H);

    vec3 Cdlin = baseColor;
    float Cdlum = 0.3*Cdlin[0] + 0.6*Cdlin[1]  + 0.1*Cdlin[2]; // luminance approximation

    vec3 Ctint = Cdlum > 0 ? Cdlin/Cdlum : vec3(1.0f); // normalize to isolate hue and sat.
    vec3 Cspec0 = mix(specular*0.08f*mix(vec3(1.0f), Ctint, specularTint), Cdlin, metallic);
    vec3 Csheen = mix(vec3(1.0f), Ctint, sheenTint);

    // Disney diffuse
    float FL = schlickFresnel(NdotL);
    float FV = schlickFresnel(NdotV);
    float Fd90 = 0.5f + 2.0f * LdotH*LdotH * roughness;
    float Fd = mix(1.0f, Fd90, FL) * mix(1.0f, Fd90, FV);

    // Hanrahan-Krueger brdf approximation of subsurface scattering
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
    float Fss90 = LdotH*LdotH*roughness;
    float Fss = mix(1.0f, Fss90, FL) * mix(1.0f, Fss90, FV);
    float ss = 1.25f * (Fss * (1.0f / (NdotL + NdotV) - 0.5f) + 0.5f);

    // Specular lobe
    float aspect = sqrt(1-(anisotropic*0.9f));
    float ax = max(0.001f, sqr(roughness)/aspect);
    float ay = max(0.001f, sqr(roughness)*aspect);
    float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = schlickFresnel(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1.0f), FH);
    float Gs;
    Gs  = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
    Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

    // Sheen
    vec3 Fsheen = FH * sheen * Csheen;

    // Clearcoat lobe (ior = 1.5 -> F0 = 0.04)
    float Dr = GTR1(NdotH, mix(0.1f,0.001f,clearCoatGloss));
    float Fr = mix(0.04f, 1.0f, FH);
    float Gr = smithG_GGX(NdotL, 0.25f) * smithG_GGX(NdotV, 0.25f);

    results.diffuse = ((1/PI) * mix(Fd, ss, subsurface) * Cdlin + Fsheen) * (1-metallic);
    results.specular = Gs*Fs*Ds;
    //float denominator = 4.0 * NdotV * NdotL + 0.0001;
    //results.specular = Gs*Fs*Ds / denominator;
    results.clearCoat = vec3(0.25f*clearCoat*Gr*Fr*Dr);

    return results;
}