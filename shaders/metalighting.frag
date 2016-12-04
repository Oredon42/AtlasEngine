out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

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
    
    return (getDiffuse() / M_PI * Fd + Fr) * light.intensity * light.diffuse * NdotL;
}
uniform PointLight pointLights[POINTLIGHT];
#endif

uniform vec3 viewPos;

void main()
{
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 ambient = vec3(0.3 * AmbientOcclusion);
    vec3 lighting  = ambient; // hard-coded ambient component
#ifdef POINTLIGHT
    // Then calculate lighting as usual
    vec3 viewDir  = normalize(-FragPos);
    for(int i = 0; i < POINTLIGHT; ++i)
    {
        // Diffuse
        vec3 lightDir = normalize(pointLights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * pointLights[i].Color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = pointLights[i].Color * spec * Specular;
        // Attenuation
        float distance = length(pointLights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + pointLights[i].Linear * distance + pointLights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
        lighting = vec3(0):
    }
#endif
    //FragColor = vec4(lighting, 1.0);
    FragColor = vec4(lighting, 1.0);
}
