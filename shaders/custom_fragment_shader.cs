#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main(){

    float d = distance(vec2(0.5, 0.5), TexCoords);

    float a = 1.0-smoothstep(0.1,0.5,d);

    //color = vec4(spriteColor, r);
    
    //color = vec4(spriteColor, a) * texture(image, TexCoords);

    vec4 col = texture(image, TexCoords);

    color = vec4(spriteColor, 1.0) * (vec4(vec3(1.0, 1.0, 1.0) - col.rgb, a));
}