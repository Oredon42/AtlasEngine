#version 330 core
in vec4 FragPos;

uniform vec3 position;
uniform float depth_map_far;

void main()
{
    float lightDistance = distance(FragPos.xyz, position);
    lightDistance = lightDistance / depth_map_far;
    
    gl_FragDepth = lightDistance;
}  
