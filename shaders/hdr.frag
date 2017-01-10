layout (location = 0) out vec3 FragColor;
layout (location = 1) out float out_exposure;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D exposure;
uniform float L_avg;
uniform float L_max;

#ifdef CHROMATIC_ABERRATION
vec3 computeChromaticAberration()
{
    float rValue = texture(scene, vec2(TexCoords.x + 0.001, TexCoords.y)).r;
    float gValue = texture(scene, TexCoords).g;
    float bValue = texture(scene, vec2(TexCoords.x - 0.001, TexCoords.y)).b;
    return vec3(rValue, gValue, bValue);
}
#endif


void main()
{
    const float inv_gamma = 1.0/2.2;
    const float a = 0.18;
    
#ifdef CHROMATIC_ABERRATION
    vec3 hdrColor = computeChromaticAberration();
#else
    vec3 hdrColor = texture(scene, TexCoords).rgb;
#endif

#ifdef BLOOM
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;
#endif
    
    vec3 result;
    float new_exposure = 1;

#ifdef ADAPTATION
    float old_exposure = texture(exposure, TexCoords).r;
    float L = max(dot(hdrColor, vec3(0.2126f, 0.7152f, 0.0722f)), 0);
    float L_scaled = a / L_avg * L;
    new_exposure = (L_scaled * (1+L_scaled/(L_max*L_max)))/(1+L_scaled);
    new_exposure = min(0.7, max(0.3, old_exposure + (new_exposure - old_exposure) * 0.1));
#endif
        
#ifdef HDR
    result = vec3(1.0) - exp(-hdrColor * new_exposure);
    result = pow(result, vec3(inv_gamma));
#else
    result = hdrColor;
#endif
    
    FragColor = result;
    out_exposure = new_exposure;
}

