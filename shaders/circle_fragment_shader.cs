#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 spriteColor;

void main(){

    float d = distance(vec2(0.5, 0.5), TexCoords);

    float a = 1.0-step(0.5,d);

    color = vec4(spriteColor.rgb, a*spriteColor.a);
}