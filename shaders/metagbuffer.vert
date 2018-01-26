layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
    layout (location = 2) in vec2 TexCoords;
#endif
#ifdef NORMAL
    layout (location = 3) in vec3 Tangent;
    layout (location = 4) in vec3 Bitangent;
#endif
#if defined(ANIMATED) || defined(NORMAL)
    layout (location = 5) in ivec4 BoneIDs;
    layout (location = 6) in vec4 Weights;
#endif
#if defined(ANIMATED) || !defined(NORMAL)
    layout (location = 3) in ivec4 BoneIDs;
    layout (location = 4) in vec4 Weights;
#endif

out VS_OUT
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
} vs_out;

uniform mat4 model;
uniform mat3 inverted_transposed_model;
uniform mat4 view;
uniform mat4 projection;
uniform float far;
uniform float near;

#ifdef ANIMATED
    const int MAX_BONES = 100;
    uniform mat4 gBones[MAX_BONES];
#endif

float normalizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
#ifdef ANIMATED
    mat4 bone_transform = gBones[BoneIDs[0]] * Weights[0];
    bone_transform += gBones[BoneIDs[1]] * Weights[1];
    bone_transform += gBones[BoneIDs[2]] * Weights[2];
    bone_transform += gBones[BoneIDs[3]] * Weights[3];

    vec4 pos_L = bone_transform * vec4(Position, 1.0);
    vec4 normal_L = bone_transform * vec4(Normal, 0.0);
#else
    vec4 pos_L = vec4(Position, 1.0);
    vec3 normal_L = inverted_transposed_model * Normal;
#endif

    gl_Position = projection * view * model * pos_L;

    vs_out.frag_pos = vec4((model * pos_L).xyz, normalizeDepth(gl_Position.z));
#ifdef NORMAL
    vs_out.TBN = mat3(Tangent, Bitangent, normal_L.xyz);
#else
    vs_out.normal = normal_L.xyz;
#endif

#if defined(TEXTURE) || defined(NORMAL) || defined(SPECULAR)
    vs_out.tex_coords = TexCoords;
#endif
}
