layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;

in VS_OUT
{
    vec4 frag_pos;
#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
    vec2 tex_coords;
#endif
#ifdef NORMAL
    mat3 TBN;
#else
    vec3 normal;
#endif
} fs_in;

struct Material
{
#ifdef TEXTURE
    sampler2D texture_diffuse1;
#else
    vec3 color;
#endif
#ifdef SPECULAR
    sampler2D texture_specular1;
#else
    vec3 specular;
#endif
#ifdef NORMAL
    sampler2D texture_normal1;
#endif
    float shininess;
    float roughness;
    float metalness;
    float refraction;
};

uniform Material material;

vec3 getDiffuse()
{
#ifdef TEXTURE
    return vec3(texture(material.texture_diffuse1, fs_in.tex_coords));
#else
    return material.color;
#endif
}
vec3 getSpecular()
{
#ifdef SPECULAR
    return vec3(texture(material.texture_specular1, fs_in.tex_coords));
#else
    return material.specular;
#endif
}
vec3 getNormal()
{
#ifdef NORMAL
    return fs_in.TBN * vec3(texture(material.texture_normal1, fs_in.tex_coords)) * 2.0 - 1.0;
#else
    return fs_in.normal;
#endif
}
float getRoughness()
{
    return material.roughness;
}
float getMetalness()
{
    return material.metalness;
}
float getRefraction()
{
    return material.refraction;
}

void main()
{
    gPositionDepth = fs_in.frag_pos;
    gNormal = getNormal();
    gAlbedoSpec.rgb = getDiffuse();
    gAlbedoSpec.a = getSpecular().x;
    gMaterial = vec3(material.roughness, material.refraction, material.metalness);
}
