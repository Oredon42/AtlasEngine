#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float key;
uniform float avg_luminance;
uniform bool hdr;
uniform bool bloom;

void main()
{
    const float gamma = 2.2;
    //vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    //  Chromatic aberration
    float rValue = texture(scene, vec2(TexCoords.x + 0.001, TexCoords.y)).r;
    float gValue = texture(scene, TexCoords).g;
    float bValue = texture(scene, vec2(TexCoords.x - 0.001, TexCoords.y)).b;
    vec3 hdrColor = vec3(rValue, gValue, bValue);

    if(bloom)
        hdrColor += bloomColor;
    
    vec3 result;
    if(hdr)
    {
        luminance = max(dot(lighting, vec3(0.2126f, 0.7152f, 0.0722f)), 0);
        lscaled = key * luminance / avg_luminance;
        result = vec3(1.0) - exp(-hdrColor * exposure);
        result = pow(result, vec3(1.0 / gamma));
    }
    else
    {
        result = hdrColor;
    }
    
    luminance =
    
    FragColor = vec4(result, 1.0f);
}
