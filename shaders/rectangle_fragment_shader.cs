#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 spriteColor;

void main(){
    
    color = spriteColor;
}