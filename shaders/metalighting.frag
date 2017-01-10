
//out vec4 FragColor;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec2 Brightness;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform sampler2D AO;

uniform float M_PI = 3.1415926535897932384626433832795;

#ifdef POINTLIGHT
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    float intensity;
    vec3 color;
};
uniform PointLight pointLights[POINTLIGHT];
#endif

#ifdef DIRLIGHT
struct DirLight {
    float intensity;
    vec3 color;
    vec3 direction;
};
uniform DirLight dirLights[DIRLIGHT];
#endif

float smoothDistanceAtt(float squaredDistance , float invSqrAttRadius)
{
    float factor = squaredDistance * invSqrAttRadius;
    float smoothFactor = max(1.0f - factor * factor, 0.0f);
    return smoothFactor * smoothFactor;
}

float getDistanceAtt(vec3 unormalizedLightVector, float invSqrAttRadius)
{
    float sqrDist = dot(unormalizedLightVector , unormalizedLightVector);
    float attenuation = 1.0 / (max(sqrDist, 0.01*0.01));
    attenuation *= smoothDistanceAtt(sqrDist , invSqrAttRadius);
    return attenuation;
}

vec3 F_Schlick(vec3 f0, float f90, float u)
{
    return f0 + (f90 - f0) * pow(1.f - u, 5.f);
}

float chiGGX(float v)
{
    return v > 0.0f ? 1.0f : 0.0f;
}

float GGX_PartialGeometryTerm(vec3 v, vec3 n, vec3 h, float alpha)
{
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float chi = chiGGX(VdotH / clamp(dot(v, n),0.0,1.0));
    VdotH = VdotH * VdotH;
    
    float tan2 = (1-VdotH)/VdotH;
    return (chi * 2) / (1 + sqrt(1+alpha*alpha*tan2));
}


float D_GGX ( float NdotH , float m )
{
    float m2 = m * m ;
    float NdotH2 = NdotH * NdotH;
    float f = NdotH2 * m2 + (1 - NdotH2);
    return (chiGGX(NdotH) * m2) / (f * f * M_PI);
}


float Fr_DisneyDiffuse( float NdotV , float NdotL , float LdotH , float linearRoughness)
{
    float energyBias = linearRoughness * 0.5;
    float energyFactor = (1.0f - linearRoughness) * 1.0 + linearRoughness * (1.0 / 1.51);
    float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    vec3 f0 = vec3(1.0f);
    float lightScatter = F_Schlick(f0, fd90, NdotL).r;
    float viewScatter = F_Schlick(f0, fd90, NdotV).r;
    
    return lightScatter * viewScatter * energyFactor;
}


#ifdef POINTLIGHT
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 V, vec3 N, float NdotV, vec3 F0, float roughness, float linearRoughness, vec3 color)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);
    float LdotH = max(dot(L, H), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    
    //Specular
    vec3 F = F_Schlick(F0, 1, LdotH);
    float Vis = GGX_PartialGeometryTerm(V, N, H, roughness) * GGX_PartialGeometryTerm(L, N, H, roughness);
    float D = D_GGX(NdotH, roughness);
    vec3 Fr =  D * F * Vis / (4*NdotV*NdotH + 0.05);
    
    //Diffuse
    float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, linearRoughness);
    
    //  Attenuation
    vec3 dist = light.position - fragPos;
    
    float attenuation = getDistanceAtt(dist, light.quadratic);
    
    return (color / M_PI * Fd + Fr) * NdotL * light.intensity;
}
#endif

#ifdef DIRLIGHT
vec3 CalcDirLight(DirLight light, vec3 fragPos, vec3 V, vec3 N, float NdotV, vec3 F0, float roughness, float linearRoughness, vec3 color)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);
    float LdotH = max(dot(L, H), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    
    //Specular
    vec3 F = F_Schlick(F0, 1, LdotH);
    float Vis = GGX_PartialGeometryTerm(V, N, H, roughness) * GGX_PartialGeometryTerm(L, N, H, roughness);
    float D = D_GGX(NdotH, roughness);
    vec3 Fr =  D * F * Vis / (4*NdotV*NdotH + 0.05);
    
    //Diffuse
    float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, linearRoughness);
    
    //  Attenuation
    vec3 dist = light.position - fragPos;
    
    float attenuation = getDistanceAtt(dist, light.quadratic);
    
    return (color / M_PI * Fd + Fr) * NdotL * light.intensity;
}
#endif

uniform vec3 viewPos;

const float gamma = 2.2;
void main()
{
    float depth = texture(gPositionDepth, TexCoords).a;
    vec3 FragPos = vec3(texture(gPositionDepth, TexCoords).rgb);
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 V = normalize(-FragPos);
    
    float roughness = texture(gMaterial, TexCoords).r;
    float linearRoughness = pow(roughness, 4);
    float ior = texture(gMaterial, TexCoords).g;
    float metalness = texture(gMaterial, TexCoords).b;

    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    color = pow(color, vec3(gamma));
    
    float AmbientOcclusion = texture(AO, TexCoords).r;
    
    vec3 F0 = vec3(0.16 * ior * ior);
    F0 = (1.0f - metalness) * F0 + metalness * color;
    
    float NdotV = abs(dot(N , V)) + 1e-5f;
    
    vec3 ambient = vec3(0.3 * AmbientOcclusion);
    vec3 lighting = vec3(0);
    
#ifdef POINTLIGHT
    for(int i = 0; i < POINTLIGHT; i++)
        lighting += CalcPointLight(pointLights[i], FragPos, V, N, NdotV, F0, roughness, linearRoughness, color);
#endif
#ifdef DIRLIGHT
    for(int i = 0; i < DIRLIGHT; i++)
        lighting += CalcDirLight(dirLights[i], FragPos, V, N, NdotV, F0, roughness, linearRoughness, color);
#endif
    lighting *= ambient;
    
    Brightness = vec2(0.0);
    float brightness = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);
    
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(brightness > 1.0)
        BrightColor = vec4(lighting, 1.0);
    
    Brightness = vec2(brightness);
    
    FragColor = vec3(max(lighting, vec3(0)));
}
