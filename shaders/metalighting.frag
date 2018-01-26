layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 BrightColor;
layout (location = 2) out float Brightness;

in vec2 TexCoords;

uniform mat4 light_space_matrix;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform sampler2D AO;
uniform samplerCube shadows;

uniform float far_plane;

uniform float M_PI = 3.1415926535897932384626433832795;

#ifdef DIRLIGHT
struct DirLight
{
    float intensity;
    vec3 color;
    vec3 direction;

    mat4 light_space_matrix;
    sampler2D depth_map;
};
uniform DirLight dirLights[DIRLIGHT];
#endif

#ifdef POINTLIGHT
struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    float intensity;
    vec3 color;

    float depth_map_near;
    float depth_map_far;
    samplerCube depth_map;
};
uniform PointLight pointLights[POINTLIGHT];
#endif

#ifdef SPOTLIGHT
struct SpotLight
{
    vec3 position;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;

    float intensity;
    vec3 color;

    float cut_off;
    float outer_cut_off;

    mat4 light_space_matrix;
    sampler2D depth_map;

};
uniform SpotLight spotLights[SPOTLIGHT];
#endif

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;
  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = M_PI * denom * denom;
  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;
  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
return (1.0 - F0) * pow(1.0 - cosTheta, 5.0) + F0;
}

in mat4 viewspaceLight;
#ifdef DIRLIGHT
vec3 CalcDirLight(DirLight light, vec3 frag_pos, vec3 V, vec3 N, vec3 F0, float roughness, float metalness, vec3 color)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);
    vec3 radiance = light.color;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;
    float NdotL = max(dot(N, L), 0.0);

    //  Shadow calculation
    vec4 light_point = light.light_space_matrix * vec4(texture(gPositionDepth, TexCoords).rgb, 1.0);
    vec3 projCoords = light_point.xyz / light_point.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(light.depth_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - NdotL), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(light.depth_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(light.depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return (kD * color / M_PI + brdf) * radiance * light.intensity * NdotL * (1-shadow);
}
#endif
#ifdef POINTLIGHT

vec3 CalcPointLight(PointLight light, vec3 frag_pos, vec3 V, vec3 N, vec3 F0, float roughness, float metalness, vec3 color)
{
    vec3 L = normalize(light.position - frag_pos);
    vec3 H = normalize(V + L);
    float distance = distance(light.position, frag_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;
    float NdotL = max(dot(N, L), 0.0);

    vec3 fragToLight = frag_pos - light.position;
    float closestDepth = texture(light.depth_map, fragToLight).r;
    closestDepth *= light.depth_map_far;
    float currentDepth = length(fragToLight);
    float bias = 0.5;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return (kD * color / M_PI + brdf) * radiance * light.intensity * NdotL * (1-shadow);
}
#endif

#ifdef SPOTLIGHT
vec3 CalcSpotLight(SpotLight light, vec3 frag_pos, vec3 V, vec3 N, vec3 F0, float roughness, float metalness, vec3 color)
{
    vec3 L = normalize(light.position - frag_pos);
    vec3 H = normalize(V + L);
    float distance = distance(light.position, frag_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 brdf = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;
    float NdotL = max(dot(N, L), 0.0);

    //	Cone intensity
    // Spotlight intensity
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float cone_intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    //  Shadow calculation
    vec4 light_point = light.light_space_matrix * vec4(texture(gPositionDepth, TexCoords).rgb, 1.0);
    vec3 projCoords = light_point.xyz / light_point.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(light.depth_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - NdotL), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(light.depth_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(light.depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return (kD * color / M_PI + brdf) * radiance * light.intensity * NdotL * cone_intensity * attenuation * (1-shadow);
}
#endif

uniform vec3 viewPos;

const float gamma = 2.2;
void main()
{
    float depth = texture(gPositionDepth, TexCoords).a;
    vec3 frag_pos = texture(gPositionDepth, TexCoords).rgb;
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 V = normalize(viewPos - frag_pos);

    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    float roughness = texture(gMaterial, TexCoords).r;
    float ior = texture(gMaterial, TexCoords).g;
    float metalness = texture(gMaterial, TexCoords).b;

    float AmbientOcclusion = texture(AO, TexCoords).r;

    vec3 F0 = vec3(0.16 * ior * ior);
    F0 = metalness * color + F0 * (1.0f - metalness);

    float NdotV = abs(dot(N , V)) + 1e-5f;

    vec3 ambient = vec3(0.3 * AmbientOcclusion);
    vec3 lighting = vec3(0);

#ifdef POINTLIGHT
    for(int i = 0; i < POINTLIGHT; i++)
        lighting += CalcPointLight(pointLights[i], frag_pos, V, N, F0, roughness, metalness, color);
#endif
#ifdef DIRLIGHT
    for(int i = 0; i < DIRLIGHT; i++)
        lighting += CalcDirLight(dirLights[i], frag_pos, V, N, F0, roughness, metalness, color);
#endif
#ifdef SPOTLIGHT
    for(int i = 0; i < SPOTLIGHT; i++)
        lighting += CalcSpotLight(spotLights[i], frag_pos, V, N, F0, roughness, metalness, color);
#endif
    lighting *= ambient;

    Brightness = 0.0;
    float brightness = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);

    BrightColor = vec3(0.0);

    if(brightness > 1.0)
        BrightColor = lighting;

    Brightness = brightness;

    FragColor = vec3(max(lighting, vec3(0)));
    //FragColor = vec3(texture(pointLights[0].depth_map, frag_pos - pointLights[0].position).r, 0, 0);
    //FragColor = vec3(texture(AO, TexCoords).rgb);
//FragColor = vec3(texture(gNormal, TexCoords).rgb);
}
