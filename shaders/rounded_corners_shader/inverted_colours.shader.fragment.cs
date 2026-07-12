#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;

void main(){

    vec4 texture_colour = texture(image, TexCoords);

    color = spriteColor * vec4(1.0-texture_colour.r,1.0- texture_colour.g,1.0- texture_colour.b,texture_colour.a);
}
