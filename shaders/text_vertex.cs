#version 450 core
layout(location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

//Is this a built in variable?
out vec2 TexCoords;

//Projection matrix?
uniform mat4 projection;

void main(){
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}