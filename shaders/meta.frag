in vec3 FragPos;
in vec3 Normal;
#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
in vec2 TexCoords;
#endif

out vec4 color;

struct Material
{
#ifdef TEXTURE
    sampler2D texture_diffuse1;
#else
    vec3 diffuse;
#endif
#ifdef SPECULAR
    sampler2D texture_specular1;
#else
    vec3 specular;
#endif
    float shininess;
    float roughness;
    float metalness;
    float refraction;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

#ifdef DIRLIGHT
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}
uniform DirLight dirLights[DIRLIGHT];
#endif

#ifdef POINTLIGHT
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float specular = 0.0;

    if(NdotL > 0.0)
    {
        // intermediary values
        vec3 halfVector = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfVector), 0.0);
        float NdotV = max(dot(normal, viewDir), 0.0);
        float VdotH = max(dot(viewDir, halfVector), 0.0);
        float mSquared = material.roughness * material.roughness;

        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));

        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);

        // fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - material.refraction);
        fresnel += material.refraction;

        specular = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
    }

    return light.diffuse * NdotL * (material.metalness + /*specular*/1 * (1.0 - material.metalness));
}
uniform PointLight pointLights[POINTLIGHT];
#endif

#ifdef SPOTLIGHT
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}
uniform SpotLight spotLights[SPOTLIGHT];
#endif

uniform vec3 viewPos;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0, 0, 0);

#ifdef DIRLIGHT
    for(int i = 0; i < DIRLIGHT; i++)
        result += CalcDirLight(dirLights[i], normal, viewDir);
#endif
#ifdef POINTLIGHT
    for(int i = 0; i < POINTLIGHT; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
#endif
#ifdef SPOTLIGHT
    for(int i = 0; i < SPOTLIGHT; i++)
        result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir);
#endif

    color = vec4(result, 1.0);
}
