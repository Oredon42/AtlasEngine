#version 330 core
out vec2 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform vec2 size;

void main()
{
    vec2 offset = 1/(2*size);
    FragColor = vec2(TexCoords.x, TexCoords.y);
    FragColor = vec2(texture(image, TexCoords).rg);
    FragColor = vec2((texture(image, TexCoords).r
                      +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y)).r
                      +texture(image, vec2(TexCoords.x, TexCoords.y - offset.y)).r
                      +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y - offset.y)).r
                      +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y)).r
                      +texture(image, vec2(TexCoords.x, TexCoords.y + offset.y)).r
                      +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y + offset.y)).r
                      +texture(image, vec2(TexCoords.x - offset.x, TexCoords.y + offset.y)).r
                      +texture(image, vec2(TexCoords.x + offset.x, TexCoords.y - offset.y)).r)/9,
                     max(max(max(max(max(max(max(max(
                                                    texture(image, TexCoords).g,
                                                     texture(image, vec2(TexCoords.x - offset.x, TexCoords.y)).g),
                                                 texture(image, vec2(TexCoords.x, TexCoords.y - offset.y)).g),
                                             texture(image, vec2(TexCoords.x - offset.x, TexCoords.y - offset.y)).g),
                                         texture(image, vec2(TexCoords.x + offset.x, TexCoords.y)).g),
                                     texture(image, vec2(TexCoords.x, TexCoords.y + offset.y)).g),
                                 texture(image, vec2(TexCoords.x + offset.x, TexCoords.y + offset.y)).g),
                             texture(image, vec2(TexCoords.x - offset.x, TexCoords.y + offset.y)).g),
                         texture(image, vec2(TexCoords.x + offset.x, TexCoords.y - offset.y)).g));
}
