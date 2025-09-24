#version 450 core
layout(location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

//Is this a built in variable?
out vec2 TexCoords;

//The matrix
uniform mat4 model;

//Projection matrix?
uniform mat4 projection;
uniform vec2 sample_position;
uniform vec2 sample_size;

void main(){
    TexCoords = vertex.zw+sample_position;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}