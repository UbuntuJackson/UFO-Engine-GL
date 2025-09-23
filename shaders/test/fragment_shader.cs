#version 330 core
out vec4 frag_colour;
uniform vec2 screen_size;

void main(){
    frag_colour = vec4(gl_FragCoord.x/screen_size.x, gl_FragCoord.y/screen_size.y, 0.0, 1.0);
}