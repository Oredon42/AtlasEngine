#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform int size;

void main()
{
    float offset = 1/(size*3);
    FragColor = (texture(image, TexCoords).r
                +texture(image, vec2(TexCoords.x - offset, TexCoords.y)).r
                +texture(image, vec2(TexCoords.x, TexCoords.y - offset)).r
                +texture(image, vec2(TexCoords.x - offset, TexCoords.y - offset)).r
                +texture(image, vec2(TexCoords.x + offset, TexCoords.y)).r
                +texture(image, vec2(TexCoords.x, TexCoords.y + offset)).r
                +texture(image, vec2(TexCoords.x + offset, TexCoords.y + offset)).r
                +texture(image, vec2(TexCoords.x - offset, TexCoords.y + offset)).r
                +texture(image, vec2(TexCoords.x + offset, TexCoords.y - offset)).r);
}
