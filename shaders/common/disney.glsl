float GTR1(float NdotH, float a)
{
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

float anisotropicGTR2(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
    return 1 / (PI * ax*ay * pow(( pow((HdotX/ax),2) + pow((HdotY/ay),2) + NdotH*NdotH ),2));
}

float smithGGX(float alphaSquared, float ndotl, float ndotv) {
    float a = ndotv * sqrt(alphaSquared + ndotl * (ndotl - alphaSquared * ndotl));
    float b = ndotl * sqrt(alphaSquared + ndotv * (ndotv - alphaSquared * ndotv));

    return 0.5f / (a + b);
}

float anisotropicSmithGGX(float NdotV, float VdotX, float VdotY, float ax, float ay) {
    return 1 / (NdotV + sqrt(pow((VdotX * ax),2) + pow((VdotY * ay),2) + pow((NdotV),2)));
}


struct BRDFResults {
    vec3 diffuse;
    vec3 specular;
    vec3 clearCoat;
};

struct DisneyMaterial {
    vec3 baseColor;
    float subsurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
};

float schlickFresnel(float u)
{
    float m = clamp(1-u, 0, 1);
    float m2 = m*m;
    return m2*m2*m; // pow(m,5)
}

BRDFResults disneyBRDF( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, DisneyMaterial mat)
{
    BRDFResults results;
    results.diffuse = vec3(0.0f);
    results.specular = vec3(0.0f);
    results.clearCoat = vec3(0.0f);

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

    vec3 H = normalize(L+V);
    float NdotH = dot(N,H);
    float LdotH = dot(L,H);

    vec3 Cdlin = baseColor;
    float Cdlum = .3*Cdlin.x + .6*Cdlin.y  + .1*Cdlin.z; // luminance approx.

    vec3 Ctint = Cdlum > 0 ? Cdlin/Cdlum : vec3(1); // normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(specular*.08*mix(vec3(1), Ctint, specularTint), Cdlin, metallic);
    vec3 Csheen = mix(vec3(1), Ctint, sheenTint);

    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float FL = schlickFresnel(NdotL);
    float FV = schlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH*LdotH * roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
    float Fss90 = LdotH*LdotH*roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);

    // specular
    float aspect = sqrt(1-anisotropic*.9);
    float ax = max(.001, pow((roughness)/aspect,2));
    float ay = max(.001, pow((roughness)*aspect,2));
    float Ds = anisotropicGTR2(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = schlickFresnel(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1), FH);
    float Gs;
    Gs  = anisotropicSmithGGX(NdotL, dot(L, X), dot(L, Y), ax, ay);
    Gs *= anisotropicSmithGGX(NdotV, dot(V, X), dot(V, Y), ax, ay);

    // sheen
    vec3 Fsheen = FH * sheen * Csheen;

    // clearcoat (ior = 1.5 -> F0 = 0.04)
    float Dr = GTR1(NdotH, mix(.1,.001,clearCoatGloss));
    float Fr = mix(.04, 1.0, FH);
    float Gr = smithGGX(NdotL, NdotV, .25);

    results.diffuse = ((1/PI) * mix(Fd, ss, subsurface)*Cdlin + Fsheen) * (1-metallic);
    results.specular = Gs*Fs*Ds;
    results.clearCoat = vec3(.25*clearCoat*Gr*Fr*Dr); // ?

    return results;
}