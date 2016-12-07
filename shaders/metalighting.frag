
//out vec4 FragColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out float Brightness;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform float M_PI = 3.1415926535897932384626433832795;

#ifdef POINTLIGHT
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    float intensity;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#endif

vec3 F_Schlick(vec3 f0, float f90, float u)
{
    return f0 + ( f90 - f0 ) * pow(1.f - u , 5.f);
}

float chiGGX(float v) {
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
    float viewScatter = F_Schlick( f0, fd90, NdotV).r;
    
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
    return (color / M_PI * Fd + Fr) * light.intensity * light.diffuse * NdotL;
}
uniform PointLight pointLights[POINTLIGHT];
#endif

uniform vec3 viewPos;

void main()
{
    // Retrieve data from gbuffer
    float depth = texture(gPositionDepth, TexCoords).a;
    //vec3 FragPos = vec3(gl_FragCoord.x, gl_FragCoord.y, 1) * depth;
    vec3 FragPos = vec3(texture(gPositionDepth, TexCoords).rgb);
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 V = normalize(-FragPos);
    
    float roughness = texture(gPositionDepth, TexCoords).r;
    roughness = 0.1;
    float linearRoughness = pow(roughness, 4);
    float ior = texture(gPositionDepth, TexCoords).b;
    ior = 1.;
    float metalness = texture(gPositionDepth, TexCoords).g;
    metalness = 1.;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 F0 = vec3(0.16 * ior * ior);
    F0 = (1.0f - metalness) * F0 + metalness * color;
    
    float NdotV = abs(dot(N , V)) + 1e-5f; // avoid artifact
    
    vec3 ambient = vec3(0.3 * AmbientOcclusion);
    vec3 lighting  = ambient; // hard-coded ambient component
#ifdef POINTLIGHT
    // Then calculate lighting as usual
    for(int i = 0; i < POINTLIGHT; i++)
        lighting += CalcPointLight(pointLights[i], FragPos, V, N, NdotV, F0, roughness, linearRoughness, color);
#endif
    Brightness = 0.0;
    float brightness = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);
    
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(brightness > 1.0)
        BrightColor = vec4(lighting, 1.0);
    
    Brightness = brightness;
    
    FragColor = vec4(max(lighting, vec3(0)), 1.0);
}
